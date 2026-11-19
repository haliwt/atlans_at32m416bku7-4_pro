#include "bsp.h"


#define PTC_OVER_TEMP_THRESHOLD_MV  1200  // 示例：假设 98度 对应的采样电压为 1200mV (请根据您的PTC特性曲线调整)


/**
 * @brief  获取 ADC2 通道 5 (PA5 - PTC电压检测) 的转换电压值（带超时保护）
 * @param  *voltage_mv: 指向存储电压值的变量 (单位: mV)
 * @retval 0: 成功, -1: 超时错误
 */
int8_t get_adc2_in5_ptc_voltage_mv(uint16_t *voltage_mv)
{
  uint16_t adc_val = 0;
  volatile uint32_t timeout = 150000; // 180MHz 主频下约 7ms ~ 10ms 超时

  /* 1. 配置 ADC2 规则组通道：选择通道 5 (PA5)，序列为 1，采样时间 239.5 个周期 */
  adc_ordinary_channel_set(ADC2, ADC_CHANNEL_5, 1, ADC_SAMPLETIME_239_5);

  /* 2. 使能软件触发普通组转换 */
  adc_ordinary_software_trigger_enable(ADC2, TRUE);

  /* 3. 等待普通通道转换完成标志 (带超时退出机制) */
  while(adc_flag_get(ADC2, ADC_OCCE_FLAG) == RESET)
  {
    if(--timeout == 0)
    {
      /* 超时保护：防止硬件异常导致死锁，直接退出 */
      return -1; 
    }
  }

  /* 4. 读取 ADC 转换结果数据 */
  adc_val = adc_ordinary_conversion_data_get(ADC2);

  /* 5. 纯整型计算电压值 (3.3V = 3300mV，12位分辨率满量程 4095) */
  *voltage_mv = (uint16_t)(((uint32_t)adc_val * 3300) / 4095);

  return 0;
}




/* 定义系统报警状态枚举 */
typedef enum {
    ALARM_STATE_NORMAL = 0,
    ALARM_STATE_OVER_TEMP = 1
} alarm_state_t;

/**
 * @brief  业务逻辑层：PTC 温度检测与高温报警判定（建议放在定时任务中，如 100ms 执行一次）
 * @param  *current_temp_mv: 输出当前采集到的 PTC 电压值 (也可在内部转为实际温度值)
 * @param  *p_alarm_state: 输出当前报警状态标志位
 * @retval 0: 成功, -1: ADC采样超时/异常
 */
int8_t ptc_temperature_monitor_handler(uint16_t *current_temp_mv, bool *p_alarm_state)
{
    /* 步骤 1：独立调用底层 ADC 采样函数，获取 PTC 电压（带超时保护） */
    if (get_adc2_in5_ptc_voltage_mv(current_temp_mv) != 0)
    {
        *p_alarm_state = ALARM_STATE_NORMAL;
        return -1; // ADC 采样超时或硬件异常
    }

    /* 步骤 2：独立执行高温报警判定逻辑 */
    if (*current_temp_mv >= PTC_OVER_TEMP_THRESHOLD_MV)
    {
        *p_alarm_state = ALARM_STATE_OVER_TEMP;
        
        /* 触发高温保护动作（如关闭 UV-C LED、开启风扇等） */
        // uvc_led_disable(); 
    }
    else
    {
        *p_alarm_state = ALARM_STATE_NORMAL;
    }

    return 0;
}


