
#include <assert.h>

#include "board.h"

#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"

#include "fsl_lpspi_mem_adapter.h"

//needed by fsl_lpspi_mem_adapter.c
void BOARD_LpspiPcsPinControl(bool isSelected)
{
	if (isSelected) {
		//assert PCS0 (PTB0)
		GPIO_PinWrite(GPIOB, 0, 0);
	} else {
		//deassert PCS0 (PTB0)
		GPIO_PinWrite(GPIOB, 0, 1);
	}
}

void BOARD_LpspiIomuxConfig(spi_pin_mode_t pinMode)
{
	switch (pinMode) {
		case kSpiIomux_SpiMode:
			//enable LPSPI1 pins
			break;
		case kSpiIomux_DefaultMode:
			//disable LPSPI1 pins
			break;
		default:
			assert(0);
			break;
	}
}

//needed by fsl_lpspi_nor_flash.c
uint32_t BOARD_GetLpspiClock(void)
{
	return CLOCK_GetIpFreq(kCLOCK_Lpspi1);
}

uint32_t BOARD_GetNorFlashBaudrate(void)
{
	//24 MHz for LPCPIS1 (isnt it way too much? will check later)
	return 24000000u;
}

LPSPI_Type *BOARD_GetLpspiForNorFlash(void)
{
	//base address of used LPSPI
	return LPSPI1;
}
