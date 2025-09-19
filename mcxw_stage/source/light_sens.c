
#include "peripherals.h"

volatile static lpadc_conv_result_t adc_res;
volatile static int adc_conv_count;

void ADC0_IRQHANDLER(void) {
	uint32_t trigger_status_flag;
	uint32_t status_flag;

	trigger_status_flag = LPADC_GetTriggerStatusFlags(ADC0_PERIPHERAL);
	status_flag = LPADC_GetStatusFlags(ADC0_PERIPHERAL);

	LPADC_ClearTriggerStatusFlags(ADC0_PERIPHERAL, trigger_status_flag);
	LPADC_ClearStatusFlags(ADC0_PERIPHERAL, status_flag);

	adc_conv_count++;

	//no meaningful data are obtained from the adc - do not know why (yet)
	if (LPADC_GetConvResult(ADC0_PERIPHERAL, &adc_res, 0) == false) {
		__BKPT();
	}
}
