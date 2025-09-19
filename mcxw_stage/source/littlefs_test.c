
#include <stdlib.h>
#include <string.h>

//#include "board.h"
//#include "app.h"

#include "fsl_nor_flash.h"
#include "fsl_lpspi_nor_flash.h"

#include "fsl_common.h"
#include "fsl_debug_console.h"

#include "fsl_crc.h"

#include "lfs.h"

#define FILENAME		"record.bin"
#define MAGIC_VALUE		0xABCD1234

//beware, this erases the filesystem
#define NOR_TEST		0

nor_handle_t nor_handle;

#if NOR_TEST
uint8_t buffer_a[4096];
uint8_t buffer_b[4096];
#endif

int nor_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);
int nor_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size);
int nor_erase(const struct lfs_config *c, lfs_block_t block);
int nor_sync(const struct lfs_config *c);

void nor_test(void);

int nor_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
	uint32_t address;

	assert(block < c->block_count);

	address = block * c->block_size + off;
	assert((address + size) <= (c->block_count * c->block_size));

	if (Nor_Flash_Read(&nor_handle, address, buffer, size) != kStatus_Success) {
		return LFS_ERR_IO;
	}

	return LFS_ERR_OK;
}

int nor_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
	uint32_t address;

	assert(block < c->block_count);

	address = block * c->block_size + off;
	assert((address + size) <= (c->block_count * c->block_size));

	assert((address % c->prog_size) == 0);
	assert((size % c->prog_size) == 0);

	while (size > 0) {
		if (Nor_Flash_Page_Program(&nor_handle, address, buffer) != kStatus_Success) {
			return LFS_ERR_IO;
		}

		address += c->prog_size;
		buffer += c->prog_size;
		size -= c->prog_size;
	}

	return LFS_ERR_OK;
}

int nor_erase(const struct lfs_config *c, lfs_block_t block)
{
	assert(block < c->block_count);

	if (Nor_Flash_Erase_Sector(&nor_handle, block * c->block_size) != kStatus_Success) {
		return LFS_ERR_IO;
	}

	return LFS_ERR_OK;
}

int nor_sync(const struct lfs_config *c)
{
	return LFS_ERR_OK;
}

#if NOR_TEST
void nor_test(void)
{
	//no dynamic memory for now, default heap is too small
	//uint8_t *buffer_a, *buffer_b;

	//buffer_a = malloc(4096);
	//assert(buffer_a != NULL);

	//buffer_b = malloc(4096);
	//assert(buffer_b != NULL);

	//erase first two sectors
	assert(Nor_Flash_Erase_Sector(&nor_handle, 0) == kStatus_Success);
	assert(Nor_Flash_Erase_Sector(&nor_handle, 4096) == kStatus_Success);

	memset(buffer_a, 0xFF, 4096);

	//check if sector 0 is erased
	memset(buffer_b, 0, 4096);
	assert(Nor_Flash_Read(&nor_handle, 0, buffer_b, 4096) == kStatus_Success);
	assert(memcmp(buffer_a, buffer_b, 4096) == 0);

	//check if sector 1 is erased
	memset(buffer_b, 0, 4096);
	assert(Nor_Flash_Read(&nor_handle, 4096, buffer_b, 4096) == kStatus_Success);
	assert(memcmp(buffer_a, buffer_b, 4096) == 0);

	//write zeros to first two sectors (32 pages)
	memset(buffer_a, 0, 256);
	for (int addr = 0; addr < (4096 * 2); addr++) {
		assert(Nor_Flash_Page_Program(&nor_handle, addr, buffer_a) == kStatus_Success);
		addr += 256;
	}

	memset(buffer_a, 0, 4096);

	//check if sector 0 contains zeros
	memset(buffer_b, 0xFF, 4096);
	assert(Nor_Flash_Read(&nor_handle, 0, buffer_b, 4096) == kStatus_Success);
	assert(memcmp(buffer_a, buffer_b, 4096) == 0);

	//check if sector 1 contains zeros
	memset(buffer_b, 0xFF, 4096);
	assert(Nor_Flash_Read(&nor_handle, 4096, buffer_b, 4096) == kStatus_Success);
	assert(memcmp(buffer_a, buffer_b, 4096) == 0);

	//erase first two sectors
	assert(Nor_Flash_Erase_Sector(&nor_handle, 0) == kStatus_Success);
	assert(Nor_Flash_Erase_Sector(&nor_handle, 4096) == kStatus_Success);

	memset(buffer_a, 0, 256);
	buffer_a[254] = 0xCA;
	buffer_a[255] = 0xFE;

	//write last page of the sector 0
	assert(Nor_Flash_Page_Program(&nor_handle, 4096 - 256, buffer_a) == kStatus_Success);

	memset(buffer_a, 0, 256);
	buffer_a[0] = 0xA5;
	buffer_a[1] = 0x5A;
	buffer_a[2] = 0x0F;
	buffer_a[3] = 0xF0;

	//write first page of the sector 1
	assert(Nor_Flash_Page_Program(&nor_handle, 4096, buffer_a) == kStatus_Success);

	//unaligned read, six bytes across the page/sector boundary
	memset(buffer_a, 0, 256);
	assert(Nor_Flash_Read(&nor_handle, 4096 - 2, buffer_a, 6) == kStatus_Success);

	assert(buffer_a[0] == 0xCA);
	assert(buffer_a[1] == 0xFE);
	assert(buffer_a[2] == 0xA5);
	assert(buffer_a[3] == 0x5A);
	assert(buffer_a[4] == 0x0F);
	assert(buffer_a[5] == 0xF0);

	//free(buffer_a);
	//free(buffer_b);
}
#endif

typedef struct {
	uint32_t magic_value;
	uint32_t run_count;
	uint32_t crc;
} RECORD;

const crc_config_t crc32_conf = {
	.polynomial = 0x4C11DB7,
	.seed = 0,
	.reflectIn = true,
	.reflectOut = true,
	.complementChecksum = true,
	.crcBits = kCrcBits32,
	.crcResult = kCrcFinalChecksum
};

int main(void)
{
	lpspi_memory_config_t lpspi_memory_config;
	nor_config_t nor_config;
	status_t status;

	struct lfs_config lfs_conf;
	lfs_t lfs;
	lfs_file_t file;

	RECORD rec;
	lfs_ssize_t read_size;

	//BOARD_InitHardware();

	PRINTF("\r\n");

	lpspi_memory_config.bytesInPageSize = 256;
	lpspi_memory_config.bytesInSectorSize = 4096;
	lpspi_memory_config.bytesInMemorySize = 8 * 1024 * 1024;

	memset(&nor_config, 0, sizeof(nor_config));
	nor_config.memControlConfig = &lpspi_memory_config;

	memset(&nor_handle, 0, sizeof(nor_handle));
	status = Nor_Flash_Init(&nor_config, &nor_handle);
	if (status != kStatus_Success)
	{
		PRINTF("\r\nNor_Flash_Init failure.");
		while (1);
	}

#if NOR_TEST
	nor_test();
#endif

	memset(&lfs_conf, 0, sizeof(lfs_conf));

	lfs_conf.read = nor_read;
	lfs_conf.prog = nor_prog;
	lfs_conf.erase = nor_erase;
	lfs_conf.sync = nor_sync;
	lfs_conf.read_size = 1;
	lfs_conf.prog_size = lpspi_memory_config.bytesInPageSize;
	lfs_conf.block_size = lpspi_memory_config.bytesInSectorSize;
	lfs_conf.block_count = lpspi_memory_config.bytesInMemorySize / lpspi_memory_config.bytesInSectorSize;
	lfs_conf.block_cycles = 500;
	lfs_conf.cache_size = 2 * lpspi_memory_config.bytesInPageSize;
	lfs_conf.lookahead_size = 16;

	memset(&lfs, 0, sizeof(lfs));

	//mount the filesystem
	PRINTF("\r\nMounting filesystem.");
	if (lfs_mount(&lfs, &lfs_conf) != LFS_ERR_OK) {
		//format the filesystem if it does not exist yet
		PRINTF("\r\nFormatting filesystem.");
		assert(lfs_format(&lfs, &lfs_conf) == LFS_ERR_OK);
		assert(lfs_mount(&lfs, &lfs_conf) == LFS_ERR_OK);
	}

	//try to open the file
	PRINTF("\r\nOpening file.");

	switch (lfs_file_open(&lfs, &file, FILENAME, LFS_O_RDWR)) {
	case LFS_ERR_NOENT:
		//the file does not exist yet, create it
		PRINTF("\r\nCreating file.");
		assert(lfs_file_open(&lfs, &file, FILENAME, LFS_O_RDWR | LFS_O_CREAT) == LFS_ERR_OK);

		rec.magic_value = MAGIC_VALUE;
		rec.run_count = 0;
		break;
	case LFS_ERR_OK:
		//the file already exists, read it and rewind it to the start
		PRINTF("\r\nReading file.");
		read_size = lfs_file_read(&lfs, &file, &rec, sizeof(rec));
		assert(read_size >= 0);

		assert(lfs_file_rewind(&lfs, &file) == LFS_ERR_OK);

		if (read_size != sizeof(rec)) {
			//the file does not contain enough data
			PRINTF("\r\nBad file size.");

			rec.magic_value = MAGIC_VALUE;
			rec.run_count = 0;
			break;
		}

		if (rec.magic_value != MAGIC_VALUE) {
			//magic value does not match, this is an unknown file format
			PRINTF("\r\nUnknown file type.");

			rec.magic_value = MAGIC_VALUE;
			rec.run_count = 0;
			break;
		}

		CRC_Init(CRC0, &crc32_conf);
		CRC_WriteData(CRC0, (uint8_t *) &rec, sizeof(rec) - sizeof(rec.crc));
		if (rec.crc != CRC_Get32bitResult(CRC0)) {
			//CRC does not match
			PRINTF("\r\nBad CRC = 0x%X", rec.crc);

			rec.magic_value = MAGIC_VALUE;
			rec.run_count = 0;
		}

		break;
	default:
		//no other lfs_file_open return values are allowed
		PRINTF("\r\nUnable to open file.");
		assert(0);
	}

	PRINTF("\r\nrun_count = %d", rec.run_count);
	rec.run_count++;

	CRC_Init(CRC0, &crc32_conf);
	CRC_WriteData(CRC0, (uint8_t *) &rec, sizeof(rec) - sizeof(rec.crc));
	rec.crc = CRC_Get32bitResult(CRC0);

	PRINTF("\r\nNew CRC = 0x%X", rec.crc);

	//write the file
	PRINTF("\r\nWriting file.");
	assert(lfs_file_write(&lfs, &file, &rec, sizeof(rec)) == sizeof(rec));

	//close the file and unmount the filesystem
	PRINTF("\r\nClosing file.");
	assert(lfs_file_close(&lfs, &file) == LFS_ERR_OK);

	PRINTF("\r\nUnmounting filesystem.");
	assert(lfs_unmount(&lfs) == LFS_ERR_OK);

	PRINTF("\r\nReset me.");

	while (1);
}
