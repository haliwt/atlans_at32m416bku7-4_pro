#ifndef __BSP_PTC_H
#define __BSP_PTC_H
#include "at32m412_416_wk_config.h"



#define DRY_ON()      do{PTC_GPIO_PORT ->scr = PTC_GPIO_PIN;}while(0)
#define DRY_OFF()     do{PTC_GPIO_PORT ->clr = PTC_GPIO_PIN;}while(0)

int8_t ptc_temperature_monitor_handler(uint16_t *current_temp_mv, bool *p_alarm_state);



#endif 


