
#include "peripherals.h"

typedef enum {
	PWM_DUTY_A,
	PWM_DUTY_B,
	PWM_DUTY_C,
	PWM_DUTY_D
} PWM_DUTY;

void GPIOC_INT_0_IRQHANDLER(void) {
	static PWM_DUTY pwm_duty = PWM_DUTY_A;

	uint32_t pin_flags0 = GPIO_GpioGetInterruptChannelFlags(GPIOC, 0U);

	GPIO_GpioClearInterruptChannelFlags(GPIOC, pin_flags0, 0U);

	switch (pwm_duty) {
	case PWM_DUTY_A:
		TPM_UpdatePwmDutycycle(TPM0_PERIPHERAL, TPM0_RED_CHANNEL, kTPM_EdgeAlignedPwm, 25);
		pwm_duty = PWM_DUTY_B;
		break;
	case PWM_DUTY_B:
		TPM_UpdatePwmDutycycle(TPM0_PERIPHERAL, TPM0_RED_CHANNEL, kTPM_EdgeAlignedPwm, 50);
		pwm_duty = PWM_DUTY_C;
		break;
	case PWM_DUTY_C:
		TPM_UpdatePwmDutycycle(TPM0_PERIPHERAL, TPM0_RED_CHANNEL, kTPM_EdgeAlignedPwm, 100);
		pwm_duty = PWM_DUTY_D;
		break;
	case PWM_DUTY_D:
		TPM_UpdatePwmDutycycle(TPM0_PERIPHERAL, TPM0_RED_CHANNEL, kTPM_EdgeAlignedPwm, 0);
		pwm_duty = PWM_DUTY_A;
		break;
	default:
		break;
	}
}

void GPIOD_INT_0_IRQHANDLER(void) {
	static PWM_DUTY pwm_duty = PWM_DUTY_A;
	uint32_t pin_flags0 = GPIO_GpioGetInterruptChannelFlags(GPIOD, 0U);

	GPIO_GpioClearInterruptChannelFlags(GPIOD, pin_flags0, 0U);

	switch (pwm_duty) {
	case PWM_DUTY_A:
		TPM_UpdatePwmDutycycle(TPM0_PERIPHERAL, TPM0_GREEN_CHANNEL, kTPM_EdgeAlignedPwm, 12);
		pwm_duty = PWM_DUTY_B;
		break;
	case PWM_DUTY_B:
		TPM_UpdatePwmDutycycle(TPM0_PERIPHERAL, TPM0_GREEN_CHANNEL, kTPM_EdgeAlignedPwm, 25);
		pwm_duty = PWM_DUTY_C;
		break;
	case PWM_DUTY_C:
		TPM_UpdatePwmDutycycle(TPM0_PERIPHERAL, TPM0_GREEN_CHANNEL, kTPM_EdgeAlignedPwm, 50);
		pwm_duty = PWM_DUTY_D;
		break;
	case PWM_DUTY_D:
		TPM_UpdatePwmDutycycle(TPM0_PERIPHERAL, TPM0_GREEN_CHANNEL, kTPM_EdgeAlignedPwm, 0);
		pwm_duty = PWM_DUTY_A;
		break;
	default:
		break;
	}
}
