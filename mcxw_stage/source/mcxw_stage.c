
#include <stdio.h>

#include "board.h"
#include "clock_config.h"
#include "peripherals.h"
#include "pin_mux.h"

#include "fsl_debug_console.h"

#include "iic_test.h"
#include "littlefs_test.h"

int main(void) {
	int pass = 0;

	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();

	assert(LPADC_GetStatusFlags(ADC0_PERIPHERAL) == kLPADC_CalibrationReadyFlag);

	DbgConsole_Init(1, 115200, kSerialPort_Uart, CLOCK_GetIpFreq(kCLOCK_Lpuart1));

	//LPADC0 trigger start
	LPIT_StartTimer(LPIT0_PERIPHERAL, LPIT0_ADC_CONV_TRIG);

	while (1);

	littlefs_test();

	PRINTF("Starting...\r\n");

	while (1) {
		PRINTF("Pass %d\r\n", pass);

		iic_test();

		PRINTF("\r\n");
		pass++;

		SDK_DelayAtLeastUs(1000000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
	}
}
