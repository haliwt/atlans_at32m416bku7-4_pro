#ifndef __BSP_FAN_H
#define __BSP_FAN_H
#include "at32m412_416_wk_config.h"





void fan_set_gear(uint8_t fan_speed);

void fan_stop(void);

int8_t fan_presence_monitor_handler(uint16_t *current_fan_mv, bool *p_fan_status);



#endif 


