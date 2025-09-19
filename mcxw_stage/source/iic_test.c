
#include "peripherals.h"
#include "fsl_debug_console.h"
#include "iic_test.h"

void iic_test(void)
{
	lpi2c_master_transfer_t xfer;
    uint8_t data;

    xfer.flags = kLPI2C_TransferDefaultFlag;
    xfer.slaveAddress = 0x76;
    xfer.subaddressSize = 1;
    xfer.data = &data;
    xfer.dataSize = 1;

    //test read
    xfer.direction = kLPI2C_Read;
    xfer.subaddress = 0xD0;
    assert(LPI2C_MasterTransferBlocking(LPI2C1_PERIPHERAL, &xfer) == kStatus_Success);
    PRINTF("Read addr=0x%x, data=0x%hhx\r\n", xfer.subaddress, data);

    //test read
    xfer.direction = kLPI2C_Read;
    xfer.subaddress = 0x6E;
    assert(LPI2C_MasterTransferBlocking(LPI2C1_PERIPHERAL, &xfer) == kStatus_Success);
    PRINTF("Read addr=0x%x, data=0x%hhx\r\n", xfer.subaddress, data);

    //test write
    xfer.direction = kLPI2C_Write;
    xfer.subaddress = 0x6E;
    data = 0xA5;
    assert(LPI2C_MasterTransferBlocking(LPI2C1_PERIPHERAL, &xfer) == kStatus_Success);
    PRINTF("Write addr=0x%x, data=0x%hhx\r\n", xfer.subaddress, data);

    //test read
    xfer.direction = kLPI2C_Read;
    xfer.subaddress = 0x6E;
    assert(LPI2C_MasterTransferBlocking(LPI2C1_PERIPHERAL, &xfer) == kStatus_Success);
    PRINTF("Read addr=0x%x, data=0x%hhx\r\n", xfer.subaddress, data);
}
