
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#include "board.h"
#include "clock_config.h"
#include "peripherals.h"
#include "pin_mux.h"

volatile static lpadc_conv_result_t adc_meas;
volatile static uint32_t adc_conv_cnt;

void ADC0_IRQHANDLER(void) {
	uint32_t trig_stat_flag;
	uint32_t stat_flag;

	trig_stat_flag = LPADC_GetTriggerStatusFlags(ADC0_PERIPHERAL);
	stat_flag = LPADC_GetStatusFlags(ADC0_PERIPHERAL);

	//assert(trig_stat_flag == kLPADC_Trigger0CompletedFlag);
	//assert(stat_flag == (kLPADC_TriggerCompletionFlag | kLPADC_ResultFIFO0ReadyFlag));

	//FIFO 0 is supposed to contain 1 sample(s)
	assert(LPADC_GetConvResultCount(ADC0_PERIPHERAL, 0) == 1);

	//FIFO 1 is supposed to contain 0 sample(s)
	assert(LPADC_GetConvResultCount(ADC0_PERIPHERAL, 1) == 0);

	//read FIFO 0
	//the cast is here just to make the type check happy
	assert(LPADC_GetConvResult(ADC0_PERIPHERAL, (lpadc_conv_result_t *) &adc_meas, 0) == true);

	//FIFO 0 is supposed to contain 0 sample(s) now
	assert(LPADC_GetConvResultCount(ADC0_PERIPHERAL, 0) == 0);

	LPADC_ClearTriggerStatusFlags(ADC0_PERIPHERAL, trig_stat_flag);
	LPADC_ClearStatusFlags(ADC0_PERIPHERAL, stat_flag);

	adc_conv_cnt++;
}
