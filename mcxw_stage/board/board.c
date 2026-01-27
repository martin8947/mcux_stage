
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#include "board.h"

//pin constants and iomux calls
#include "pin_mux.h"

//LPSPI_Type and LPSPI1
#include "fsl_common.h"

//GPIO_PinWrite
#include "fsl_gpio.h"

//CLOCK_GetIpFreq and kCLOCK_Lpspi1
#include "fsl_clock.h"

//spi_pin_mode_t
#include "fsl_lpspi_mem_adapter.h"

void BOARD_LpspiPcsPinControl(bool isSelected)
{
	if (isSelected) {
		//assert chip select
		GPIO_PinWrite(PIN_INIT_BULK_LPSPI1_PCS0_GPIO, PIN_INIT_BULK_LPSPI1_PCS0_PIN, 0);
	} else {
		//deassert chip select
		GPIO_PinWrite(PIN_INIT_BULK_LPSPI1_PCS0_GPIO, PIN_INIT_BULK_LPSPI1_PCS0_PIN, 1);
	}
}

void BOARD_LpspiIomuxConfig(spi_pin_mode_t pinMode)
{
	switch (pinMode) {
		case kSpiIomux_SpiMode:
			//enable LPSPI1 pins
			pin_init_lpspi1();
			break;
		case kSpiIomux_DefaultMode:
			//disable LPSPI1 pins
			pin_deinit_lpspi1();
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
