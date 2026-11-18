#include "bsp.h"


void buzzer_sound_on(void)
{

	/* 50% duty cycle */
	 tmr_channel_value_set(TMR10, TMR_SELECT_CHANNEL_1, 125);
	
	/* make sure the timer is running and the output is enabled */
	tmr_output_enable(TMR10, TRUE);
	tmr_counter_enable(TMR10, TRUE);
	
	/* output PWM for 10ms */
	tx_thread_sleep(20);
	
	/* turn off TMR10 */
	tmr_counter_enable(TMR10, FALSE);
	tmr_output_enable(TMR10, FALSE);


}

void buzzer_sound_off(void)
{
	/* turn off TMR10 */
	tmr_counter_enable(TMR10, FALSE);
	tmr_output_enable(TMR10, FALSE);


}

