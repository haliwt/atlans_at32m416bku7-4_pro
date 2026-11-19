#include "bsp.h"

#define FAN_MISSING_THRESHOLD_MV    1000  // 阈值：1000mV (1V)，低于此值认为风扇未连接



/* 定义风扇档次枚举 */
typedef enum {
    FAN_GEAR_OFF = 0,   /* 关闭 */
    FAN_GEAR_LOW,       /* 低档 (30%) */
    FAN_GEAR_MID,       /* 中档 (60%) */
    FAN_GEAR_HIGH       /* 高档 (100%) */
} fan_gear_t;

/**
 * @brief  设置风扇调速档次
 * @param  gear: 目标档次 (FAN_GEAR_OFF / LOW / MID / HIGH)
 * @retval none
 */
void fan_set_gear(uint8_t fan_speed)
{
    uint16_t compare_value = 0;

    switch (fan_speed) {
        case FAN_GEAR_OFF:
            compare_value = 0;          // 0% 占空比
            tmr_counter_enable(TMR3, FALSE);
            break;
        case FAN_GEAR_LOW:
			
            compare_value = 4100;       // 50% 占空比 (7200 * 0.5)
            tmr_counter_enable(TMR3, TRUE);
            break;
        case FAN_GEAR_MID:
			
            compare_value = 4320;       // 60% 占空比 (7200 * 0.6)
            tmr_counter_enable(TMR3, TRUE);
            break;
        case FAN_GEAR_HIGH:
            compare_value = 7199;       // 100% 占空比
            tmr_counter_enable(TMR3, TRUE);
            break;
        default:
            compare_value = 0;
			 tmr_counter_enable(TMR3, FALSE);
            break;
    }

    /* 动态更新 TMR3 通道 2 的比较寄存器值（占空比） */
    tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, compare_value);
}


void fan_stop(void)
{
    tmr_counter_enable(TMR3, FALSE);
	tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, 0);


}

/**
 * @brief  获取 ADC2 通道 0 (PA0) 的转换电压值
 * @param  None
 * @retval 电压值 (单位: mV)
 */
int8_t get_fan_adc2_in0_voltage_mv(uint16_t *voltage_mv)
{
 uint16_t adc_val = 0;
  uint32_t timeout = 50000; // 根据主频（如64MHz）调整此超时计数值

  /* 1. 配置 ADC2 规则组通道：选择通道 0，序列为 1，采样时间 239.5 个周期 */
  adc_ordinary_channel_set(ADC2, ADC_CHANNEL_0, 1, ADC_SAMPLETIME_239_5);

  /* 2. 使能软件触发普通组转换 */
  adc_ordinary_software_trigger_enable(ADC2, TRUE);

  /* 3. 等待普通通道转换完成标志 (带超时退出机制) */
  while(adc_flag_get(ADC2, ADC_OCCE_FLAG) == RESET)
  {
    if(--timeout == 0)
    {
      /* 超时保护：清除可能残留的触发或状态，直接退出 */
      return -1; 
    }
  }

  /* 4. 读取 ADC 转换结果数据 */
  adc_val = adc_ordinary_conversion_data_get(ADC2);

  /* 5. 纯整型计算电压值 (3.3V = 3300mV，12位分辨率满量程 4095) */
  *voltage_mv = (uint16_t)(((uint32_t)adc_val * 3300) / 4095);

  return 0;
}



/* 定义风扇状态枚举 */
typedef enum {
    FAN_STATUS_NORMAL   = 0,  // 风扇正常（电压 >= 1V）
    FAN_STATUS_MISSING  = 1,  // 风扇未检测到/断开（电压 < 1V）
    FAN_STATUS_ERROR    = 0xFF // ADC 采样异常
} fan_status_t;

/**
 * @brief  风扇状态检测与报警监控任务（建议周期性调用，如 100ms 一次）
 * @param  *current_fan_mv: 输出当前采集到的风扇检测电压值
 * @param  *p_fan_status: 输出当前风扇状态标志位
 * @retval 0: 成功, -1: ADC采样失败
 */
int8_t fan_presence_monitor_handler(uint16_t *current_fan_mv, bool *p_fan_status)
{
    /* 步骤 1：独立调用底层采样获取风扇引脚电压 */
    if (get_fan_adc2_in0_voltage_mv(current_fan_mv) != 0)
    {
        *p_fan_status = FAN_STATUS_ERROR;
        return -1; // ADC 采样超时或硬件故障
    }

    /* 步骤 2：独立执行小于 1V 的缺失判定逻辑 */
    if (*current_fan_mv < FAN_MISSING_THRESHOLD_MV)
    {
        *p_fan_status = FAN_STATUS_MISSING;
        
        /* 触发报警或安全保护动作（例如：切断驱动、点亮故障灯、上报主控屏幕） */
        // trigger_fan_alarm_action();
    }
    else
    {
        *p_fan_status = FAN_STATUS_NORMAL;
    }

    return 0;
}

