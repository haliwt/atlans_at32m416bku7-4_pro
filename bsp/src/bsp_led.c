#include "bsp.h"



void led_1_blink_1s(void)
{
    static uint8_t switch_f;

	if(gpro_t.timr11_counter > 10){
		
		gpro_t.timr11_counter=0;
        buzzer_sound_on();
        switch_f = switch_f ^ 0x01;

		if(switch_f)LED_1_ON();
		else LED_1_OFF();


	}

}



