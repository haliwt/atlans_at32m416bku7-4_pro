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
#include "bsp_light.h"
#include "bsp_ntc.h"


#include "bsp_threadx.h"


/* 功能类型枚举 */
typedef enum {
    FUNC_NONE = 0,
    FUNC_DEODORIZE,   	/* 除异味（过程型） */    
    FUNC_MITE,      	/* 除螨（过程型，共用等离子模块） */
    FUNC_DISINFECT,  	/* 消毒（过程型，等离子 + UV-C灯具，门锁互锁） */
	FUNC_DRY, 			/* 干燥（维持型，常驻） */
  } care_func_t;

/* 档位枚举（对应运行时长或强度） */
typedef enum {
    GEAR_0 = 0,     /* 关闭 */
    GEAR_1 = 1,     /* 30分钟 / 低档 */
    GEAR_2 = 2,     /* 45分钟 / 中档 */
    GEAR_3 = 3      /* 60分钟 / 高档 */
} care_gear_t;





typedef struct MANI_PRO_T{

  uint8_t timr11_counter ;

  bool power_on_flag;
  uint8_t mode_counter_flag;

  uint8_t dc_power;

  int16_t aht30_temperature_value;
  int16_t aht30_humidity_value;

  /* --- 新增：ADC 采样转换后的电压值 (单位: mV) --- */
  uint16_t ptc_voltage_mv;    /* PTC 检测 ADC 电压值 */
  uint16_t fan_voltage_mv;    /* 风扇检测 ADC 电压值 */
  uint16_t light_voltage_mv;  /* 光线检测 ADC 电压值 */

  /* --- 新增：状态与报警标志位 --- */
 // uint8_t  fan_fault_flag;    /* 风扇故障标志位 (0: 正常, 1: 故障/未连接 < 1V) */
 // uint8_t  ptc_alarm_flag;    /* PTC 高温报警标志位 (0: 正常, 1: 高温报警 >= 98度) */
 // uint8_t  door_closed_flag;  /* 门状态标志位 (1: 门已关闭, 0: 门已打开/透光) */

  /* 状态与报警标志位 */
    bool     fan_fault_flag;    /* 风扇故障标志位 (开机/运行中检测 < 1V) */
    bool     ptc_alarm_flag;    /* PTC 高温报警标志位 (>= 98度) */
    bool     door_closed_flag;  /* 门状态标志位 (true: 门已关闭, false: 门已打开) */

    /* 当前运行状态 */
    care_func_t active_process_func; /* 当前运行的过程型功能 */
    uint8_t process_gear;        /* 过程型功能档位 */
    uint16_t    process_countdown_s; /* 过程型功能倒计时（秒） */
    bool        is_process_running;  /* 过程型功能是否在运行 */
    bool        is_process_paused;   /* 是否因开门导致暂停 */

    bool        is_dry_active;       /* 干燥功能是否开启 */
	bool        send_aht30_data_flag ;
    care_gear_t dry_gear;            /* 干燥档位 */

}_main_pro_t;

extern _main_pro_t  gpro_t ;

void bsp_init(void);


#endif 


