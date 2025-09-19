
#include <stdio.h>

#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"

#include "fsl_clock.h"
#include "fsl_debug_console.h"

#include "iic_test.h"

int main(void) {
	int pass = 0;

	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();

	DbgConsole_Init(1, 115200, kSerialPort_Uart, CLOCK_GetIpFreq(kCLOCK_Lpuart1));

	PRINTF("Starting...\r\n");

    LPIT_StartTimer(LPIT0_PERIPHERAL, LPIT0_ADC_CONV_TRIG);

	while (1) {
		PRINTF("Pass %d\r\n", pass);

		iic_test();

		PRINTF("\r\n");
		pass++;

		SDK_DelayAtLeastUs(1000000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
	}
}
