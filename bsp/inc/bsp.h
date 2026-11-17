#ifndef __BSP_H
#define __BSP_H
#include <string.h>
#include <stdbool.h>

#include "at32m412_416_wk_config.h"
#include "tx_api.h"


#include "wk_adc.h"
#include "wk_can.h"
#include "wk_tmr.h"
#include "wk_usart.h"
#include "wk_wdt.h"
#include "wk_gpio.h"
#include "wk_system.h"


#include "bsp_can.h"
#include "bsp_adc.h"
#include "bsp_plasma.h"
#include "bsp_aht30.h"
#include "bsp_fan.h"
#include "bsp_ptc.h"
#include "bsp_disp.h"
#include "bsp_buzzer.h"
#include "bsp_key.h"
#include "bsp_tim.h"
#include "bsp_disingfect.h"
#include "bsp_uvc.h"
#include "bsp_deodorize.h"
#include "bsp_mites.h"
#include "bsp_can_protocol.h"
#include "bsp_power.h"
#include "bsp_led.h"


#include "bsp_threadx.h"




typedef struct MANI_PRO_T{

  uint8_t timr11_counter ;




}_main_pro_t;

extern _main_pro_t  gpro_t ;


#endif 


