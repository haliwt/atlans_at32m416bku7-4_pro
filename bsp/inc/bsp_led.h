#ifndef __BSP_LED_H
#define __BSP_LED_H
#include "at32m412_416_wk_config.h"




#define LED_POWER_TOGGLE()   do{GPIOA->togr = LED_POWER_PIN;}while(0)
#define LED_POWER_OFF()      do{GPIOA->clr = LED_POWER_PIN;}while(0)

#define LED_1_ON()           do{GPIOA->scr  = LED_1_PIN;}while(0)
#define LED_1_OFF()          do{GPIOA ->clr = LED_1_PIN;}while(0)

void led_1_blink_1s(void);



#endif 

