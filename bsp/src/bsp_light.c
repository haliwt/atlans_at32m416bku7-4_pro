#include "bsp.h"

#define LIGHT_THRESHOLD_MV    1500  // 示例阈值：1500mV (低于此值认为有光即门开，具体根据硬件电路调整)


/**
 * @brief  获取 ADC2 通道 1 (PA1 - 光线检测) 的转换电压值（带超时保护）
 * @param  *voltage_mv: 指向存储电压值的变量 (单位: mV)
 * @retval 0: 成功, -1: 超时错误
 */
int8_t get_adc2_in1_light_voltage_mv(uint16_t *voltage_mv)
{
  uint16_t adc_val = 0;
  volatile uint32_t timeout = 150000; // 180MHz 主频下约 7ms ~ 10ms 超时

  /* 1. 配置 ADC2 规则组通道：选择通道 1 (PA1)，序列为 1，采样时间 239.5 个周期 */
  adc_ordinary_channel_set(ADC2, ADC_CHANNEL_1, 1, ADC_SAMPLETIME_239_5);

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




/* 定义门的状态枚举 */
typedef enum {
    DOOR_STATUS_CLOSED = 0,
    DOOR_STATUS_OPEN   = 1,
    DOOR_STATUS_ERROR  = 0xFF
} door_status_t;

/**
 * @brief  光线传感器检测任务（建议周期性调用，如 50ms 一次）
 * @param  *p_status: 输出当前门状态标志位
 * @retval 0: 成功, -1: ADC采样超时/异常
 */
int8_t door_light_detect_handler(uint8_t *p_status)
{
    uint16_t light_mv = 0;

    /* 1. 独立执行：仅负责获取 ADC 光线电压值（带超时保护） */
    if (get_adc2_in1_light_voltage_mv(&light_mv) != 0)
    {
        *p_status = DOOR_STATUS_ERROR;
        return -1; // ADC 采样异常或超时
    }

    /* 2. 独立执行：根据电压与阈值的比较，更新门状态标志位 */
    /* 假设：门打开时，光线照入传感器，电压升高（或降低，取决于光敏电阻接法） */
    if (light_mv > LIGHT_THRESHOLD_MV)
    {
        *p_status = DOOR_STATUS_OPEN;
    }
    else
    {
        *p_status = DOOR_STATUS_CLOSED;
    }

    return 0;
}

