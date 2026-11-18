#include "bsp.h"


// 任务结构体：这次我们直接检查标志位
typedef struct {
	uint32_t last_tick; 	   // 记录上一次真正运行时的系统绝对时间戳
    //volatile uint8_t *task_f; // 指向定时器置位的标志位
    //uint32_t period;        // 任务运行周期
    uint32_t period_ms;     // 存储的是 毫秒(ms) 值，更改名字更清晰
    void (*task_handler)(void);  // 任务函数
} Task_Config_t;

// --- 任务函数声明 ---
static void handler_ntc_adc_value(void);
static void handler_works_hours(void);
static void handler_read_aht30(void);
static void handler_fan_adc_detected(void);
static void handler_door_detected(void);
static void handler_mainboard_fun(void);


// 2. 任务注册表：将标志位地址与函数关联
static Task_Config_t g_tasks[] = {
    // last_tick,  period(ms),  task_handler
  
    {0,            300,        handler_works_hours},
    {0,            450,        handler_read_aht30},
    {0,            100,        handler_ntc_adc_value},         // 10ms*100=1000ms =1s
    {0,            500,        handler_fan_adc_detected},       //500 *10ms =5000ms = 5s 
    {0,            400,        handler_door_detected},
    {0,            20,         handler_mainboard_fun}             //10ms *20 = 200ms
 };


#define TASK_COUNT     (sizeof(g_tasks) / sizeof(Task_Config_t))

static void care_hardware_output_update(void);

/************************************************************************
 *
 * Function Name:void power_on_cycle_handler(void)
 * 功能:
 * 参数:无
 * 返回值:无
 *
 ************************************************************************/
void power_on_cycle_handler(void)
{
	uint8_t i ;

	// 获取当前系统的绝对时间戳
      uint32_t current_tick = tx_time_get();
    // 通过时间片轮询核心算法，分时调用各个功能模块
    for (i = 0; i < TASK_COUNT; i++) 
    {
        // 使用纯 Tick 单位进行无符号减法，完美天然支持死循环绕回（Overflow）
		if ((current_tick - g_tasks[i].last_tick) >= g_tasks[i].period_ms) 
        {
            // 【工业级进化：防轰炸饱和截断】
            // 如果卡顿/被高优先级抢占的时间超过了 2 个周期，直接对齐当前时间，放弃追赶
            if ((current_tick - g_tasks[i].last_tick) > ( g_tasks[i].period_ms * 2)) 
            {
                g_tasks[i].last_tick = current_tick;
            }
            else 
            {
                // 如果只是正常范围内的轻微抖动，滚动累加周期，死锁锁相，消除长期长跑漂移
                g_tasks[i].last_tick += g_tasks[i].period_ms;
            }
            
            // 触发对应周期的执行函数（确保不为 NULL，防止空指针崩溃）
            if (g_tasks[i].task_handler != NULL)
            {
                g_tasks[i].task_handler(); 
            }
        }
    }


}
/**
  * @brief  
  * @note  
  * @param
  * @intverl
**/
static void handler_ntc_adc_value(void)
{
	ptc_temperature_monitor_handler(&gpro_t.ptc_voltage_mv,&gpro_t.ptc_alarm_flag);

}
/**
  * @brief  fan run is ok
  * @note  
  * @param
  * @intverl
**/
static void handler_works_hours(void)
{

}
/**
  * @brief  fan run is ok
  * @note  
  * @param
  * @intverl
**/

static void handler_read_aht30(void)
{
   uint8_t check_aht30_flag; 
  check_aht30_flag = AHT30_Read_Data(&gpro_t.aht30_temperature_value, &gpro_t.aht30_humidity_value);
  if(check_aht30_flag == 0){

     gpro_t.send_aht30_data_flag = true;
  }
  else{

      gpro_t.send_aht30_data_flag = false;

  }
}
/**
  * @brief  fan run is ok
  * @note  
  * @param
  * @intverl
**/

static void handler_fan_adc_detected(void)
{
    fan_presence_monitor_handler(&gpro_t.fan_voltage_mv,&gpro_t.fan_fault_flag);
}
/**
  * @brief  fan run is ok
  * @note  
  * @param
  * @intverl
**/

static void handler_door_detected(void)
{
	door_light_detect_handler(&gpro_t.door_closed_flag);

}
/**
  * @brief  mainboard of function is runing .
  * @note  
  * @param
  * @intverl
**/
static void handler_mainboard_fun(void)
{
   care_hardware_output_update();
   
}
/************************************************************************
 *
 * Function Name: LED_Power_Breathing(void)
 * 功能:
 * 参数:无
 * 返回值:无
 *
 ************************************************************************/

/**
 * @brief  内部执行模块输出控制（根据当前状态驱动硬件：风扇、等离子、UV-C、PTC）
 */
static void care_hardware_output_update(void)
{
    /* 安全检查：如果发生 PTC 高温报警（>=98度）或风扇故障，强制关闭高危模块 */
    if (gpro_t.ptc_alarm_flag || gpro_t.fan_fault_flag)
    {
        
        return;
    }

    /* 1. 消毒功能安全互锁：若门打开（door_closed_flag == false），UV-C 必须立即强制断电并暂停计时 */
    if (gpro_t.active_process_func == FUNC_DISINFECT && !gpro_t.door_closed_flag)
    {
        uvc_led_off();//gpio_bits_write(UV_C_PORT, UV_C_PIN, FALSE); /* 强制关闭 UV-C 灯具 */
                                            //door is open is "1"
        gpro_t.is_process_paused = true;     /* 过程暂停 */
        return;
    }
    else if (gpro_t.is_process_paused && gpro_t.door_closed_flag)
    {

		//uvc_led_on();/* UV-C 灯具 关门后自动恢复运行 */
        gpro_t.is_process_paused = false;
    }

    /* 2. 过程型模块控制（除异味 / 除螨 / 消毒） */
    if (gpro_t.is_process_running && !gpro_t.is_process_paused)
    {

         /* 共用等离子模块：在除异味、除螨、消毒时均开启 */
        if((gpro_t.active_process_func == FUNC_DEODORIZE || 
            gpro_t.active_process_func == FUNC_MITE || 
            gpro_t.active_process_func == FUNC_DISINFECT) && gpro_t.process_gear !=0) 
         {
            //gpio_bits_write(PLASMA_PORT, PLASMA_PIN, TRUE);
            if(gpro_t.active_process_func == FUNC_MITE){
                PLASMA_ON();  
			}
			else{
			   PLASMA_ON();  
               NEGATIVE_ON();

			}
        }

        /* 消毒专属：开启 UV-C 灯具 */
        if (gpro_t.active_process_func == FUNC_DISINFECT && gpro_t.process_gear !=0)
        {
            uvc_led_on();//PLASMA_ON();//gpio_bits_write(UV_C_PORT, UV_C_PIN, TRUE);
            //NEGATIVE_ON();
        }

        /* 共用风扇档位策略：根据档位设置风扇 PWM 占空比 */
        switch (gpro_t.process_gear)
        {

		    case GEAR_0:

               if(gpro_t.is_dry_active == false){
                   DRY_OFF();
               }
			   else{
			       PLASMA_OFF();  
				   NEGATIVE_OFF();
			       uvc_led_off();
              }
			   fan_set_gear(0);

			break;


			case GEAR_1:
               fan_set_gear(1);/* 低档 30% 占空比 */
                break;
            case GEAR_2:
               fan_set_gear(2); /* 中档 50% 占空比 */
                break;
            case GEAR_3:
                 fan_set_gear(3);  /* 高档 100% 占空比 */
                break;
        }
    }
    else
    {
        /* 过程型功能未运行或暂停时，关闭等离子与 UV-C */
        //gpio_bits_write(PLASMA_PORT, PLASMA_PIN, FALSE);
        //gpio_bits_write(UV_C_PORT, UV_C_PIN, FALSE);
        PLASMA_OFF();  
        NEGATIVE_OFF();
		uvc_led_off();

        /* 3. 维持型功能：干燥常驻运行处理 */
        if (gpro_t.is_dry_active)
        {
            switch (gpro_t.dry_gear)
            {
                case GEAR_1: /* 静音间歇/低风量 */
                    fan_set_gear(1);//tmr_channel_value_set(TMR4, FAN_PWM_CHANNEL, 15000); 
                    break;
                case GEAR_2: /* 标准档 */
                    fan_set_gear(2);//tmr_channel_value_set(TMR4, FAN_PWM_CHANNEL, 30000); 
                    break;
                case GEAR_3: /* 强力连续 */
                    fan_set_gear(3);//tmr_channel_value_set(TMR4, FAN_PWM_CHANNEL, 59999); 
                    break;
            }
        }
        else
        {
            fan_set_gear(0);//tmr_channel_value_set(TMR4, FAN_PWM_CHANNEL, 0);
        }
    }
}


/**
 * @brief  
 * @param
 * @input 
 * @retval 
 */


