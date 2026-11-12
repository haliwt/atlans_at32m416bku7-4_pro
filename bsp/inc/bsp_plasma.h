#ifndef __BSP_PLASMA_H
#define __BSP_PLASMA_H

#include "at32m412_416_wk_config.h"



#define PLASMA_ON()   if(PLASMA_GPIO_PORT  ->scr = PLASMA_GPIO_PIN;)while(0)

#define PLASMA_OFF()  if(PLASMA_GPIO_PORT ->clr = PLASMA_GPIO_PIN;)while(0)


#define NEGATIVE_ON()  if(NEGATIVE_GPIO_PORT ->scr  = NEGATIVE_GPIO_PIN;)while(0)

#define NEGATIVE_OFF() if(NEGATIVE_GPIO_PORT ->clr  = NEGATIVE_GPIO_PIN;)while(0)


#endif 



