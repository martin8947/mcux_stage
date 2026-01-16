
#include <assert.h>
#include <stdbool.h>

#include "board.h"

#include "peripherals.h"
#include "pin_mux.h"

//needed for spi_pin_mode_t
#include "fsl_lpspi_mem_adapter.h"

void BOARD_LpspiPcsPinControl(bool isSelected)
{
	if (isSelected) {
		//assert chip select
		GPIO_PinWrite(BOARD_INITPINS_LPSPI1_PCS0_GPIO, BOARD_INITPINS_LPSPI1_PCS0_PIN, 0);
	} else {
		//deassert chip select
		GPIO_PinWrite(BOARD_INITPINS_LPSPI1_PCS0_GPIO, BOARD_INITPINS_LPSPI1_PCS0_PIN, 1);
	}
}

void BOARD_LpspiIomuxConfig(spi_pin_mode_t pinMode)
{
	switch (pinMode) {
		case kSpiIomux_SpiMode:
			//enable LPSPI1 pins
			BOARD_InitPins_LPSPI1();
			break;
		case kSpiIomux_DefaultMode:
			//disable LPSPI1 pins
			BOARD_DeinitPins_LPSPI1();
			break;
		default:
			//just in case
			assert(0);
			break;
	}
}

uint32_t BOARD_GetLpspiClock(void)
{
	return CLOCK_GetIpFreq(kCLOCK_Lpspi1);
}

uint32_t BOARD_GetNorFlashBaudrate(void)
{
	//32 MHz shall be OK for our flash
	return 32000000ul;
}

LPSPI_Type *BOARD_GetLpspiForNorFlash(void)
{
	//base address of used LPSPI
	return LPSPI1;
}
