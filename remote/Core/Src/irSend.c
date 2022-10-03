#include "IRremote.h"
#include "IRremoteInt.h"
#include "stm32f7xx_hal_tim.h"

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

void delay_us(uint32_t us)
{
	__HAL_TIM_SET_COUNTER(&htim2,0);  // set the counter value a 0
	while ((__HAL_TIM_GET_COUNTER(&htim2)) < us);  // wait for the counter to reach the us input in the parameter
}

//+=============================================================================
// Sends an IR mark for the specified number of microseconds.
// The mark output is modulated at the PWM frequency.
//
void IRsend_mark (unsigned int time)
{
	HAL_TIM_OC_Start(&htim3, TIM_CHANNEL_3); // Enable PWM output
	delay_us(time);
}

//+=============================================================================
// Leave pin off for time (given in microseconds)
// Sends an IR space for the specified number of microseconds.
// A space is no output, so the PWM output is disabled.
//
void IRsend_space (unsigned int time)
{
	HAL_TIM_OC_Stop(&htim3, TIM_CHANNEL_3); // Disable PWM output
	delay_us(time);
}

//+=============================================================================
// Enables IR output.  The khz value controls the modulation frequency in kilohertz.
// To turn the output on and off, we leave the PWM running, but connect and disconnect the output pin.
//
void IRsend_enableIROut (uint32_t khz)
{
	//------------------------------------------------------------------
	// TIM3 initialization
	//------------------------------------------------------------------

	GPIO_InitTypeDef GPIO_IR_TIMER_PWM;
	TIM_OC_InitTypeDef IR_TIMER_PWM_CH;

	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_TIM3_CLK_ENABLE();

	GPIO_IR_TIMER_PWM.Pin = GPIO_PIN_8; //ARD_D5_PWM_Pin;
	GPIO_IR_TIMER_PWM.Mode = GPIO_MODE_AF_PP;
	GPIO_IR_TIMER_PWM.Pull = GPIO_NOPULL;
	GPIO_IR_TIMER_PWM.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_IR_TIMER_PWM.Alternate = GPIO_AF2_TIM3;
	HAL_GPIO_Init(GPIOC, &GPIO_IR_TIMER_PWM); //ARD_D5_PWM_GPIO_Port

	HAL_TIM_OC_DeInit(&htim3);

	/* PWM_frequency = timer_tick_frequency / (TIM_Period + 1) */

	htim3.Instance = TIM3;
	uint32_t period = (1000 / khz) - 1;
	htim3.Init.Period = period & 0xFFFF; //vrednost v ARR (Auto-Reload Register)
	//100 MHz
	htim3.Init.Prescaler = 100 - 1; //deljenje frekvence ure, Prescaler = 99 + 1
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP; //stetje navzgor
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; //dodatni devision ure (DIV1 = 1)

	HAL_TIM_Base_Init(&htim3);
	HAL_TIM_OC_Init(&htim3); //OC = output compare

	/* PWM mode 2 = Clear on compare match */
	/* PWM mode 1 = Set on compare match */
	IR_TIMER_PWM_CH.OCMode = TIM_OCMODE_PWM1;

	/* To get proper duty cycle, you have simple equation */
	/* pulse_length = ((TIM_Period + 1) * DutyCycle) / 100 - 1 */
	/* where DutyCycle is in percent, between 0 and 100% */

	IR_TIMER_PWM_CH.Pulse = (((uint32_t) period) / 2) & 0xFFFF; //CCR (Capture Compare Register)
	IR_TIMER_PWM_CH.OCPolarity = TIM_OCPOLARITY_HIGH; //v katerem stanju zacne signal
	IR_TIMER_PWM_CH.OCNPolarity = TIM_OCNPOLARITY_HIGH; //complementary channel
	IR_TIMER_PWM_CH.OCFastMode = TIM_OCFAST_DISABLE; //fast mode disabled
	IR_TIMER_PWM_CH.OCIdleState = TIM_OCIDLESTATE_RESET; //ko je idle state je pin v reset
	IR_TIMER_PWM_CH.OCNIdleState = TIM_OCNIDLESTATE_RESET; //complementary channel

	HAL_TIM_OC_ConfigChannel(&htim3, &IR_TIMER_PWM_CH, TIM_CHANNEL_3);
	TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_3, TIM_CCxN_ENABLE | TIM_CCx_ENABLE ); //

	HAL_TIM_OC_Start(&htim3, TIM_CHANNEL_3); // start generating IR carrier


	//------------------------------------------------------------------
	// TIM2 initialization (1 Mhz => 1 tick = 1 us)
	//------------------------------------------------------------------

	__HAL_RCC_TIM2_CLK_ENABLE();
	htim2.Instance = TIM2;
	htim2.Init.Period = 0xFFFF - 1; //vrednost v ARR (Auto-Reload Register)
	//100 MHz
	htim2.Init.Prescaler = 100 - 1; //deljenje frekvence ure, Prescaler = 99 + 1
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP; //stetje navzgor
	//htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; //dodatni devision ure (DIV1 = 1)

	HAL_TIM_Base_Init(&htim2);
	//HAL_TIM_Base_Start(&htim2);
	__HAL_TIM_ENABLE(&htim2);

}
