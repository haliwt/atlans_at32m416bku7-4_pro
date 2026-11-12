#ifndef __BSP_PTC_H
#define __BSP_PTC_H
#include "at32m412_416_wk_config.h"



#define DRY_ON()      if(PTC_GPIO_PORT ->scr = PTC_GPIO_PIN;)while(0)
#define DRY_OFF()      if(PTC_GPIO_PORT ->clr = PTC_GPIO_PIN;)while(0)



#endif 


