#include "bsp.h"

/**
 * @brief  设置 UVC 灯 PWM 输出辉度
 * @param  brightness: 0 表示关闭 (0%), 30000 表示 50% 亮度
 * @retval none
 */
void uvc_led_on(void)
{
    /* 限制最大比较值不超过 ARR (59999) */
    //uint16_t brightness
	//uvc_led_set_brightness(30000); // 60000 * 50% = 30000
	  tmr_counter_enable(TMR4,TRUE);

    /* 动态更新 TMR4 通道 3 的比较寄存器值 */
    tmr_channel_value_set(TMR4, TMR_SELECT_CHANNEL_3,30000);
}

void uvc_led_off(void)
{

  tmr_counter_enable(TMR4,FALSE);
	
	  /* 动态更新 TMR4 通道 3 的比较寄存器值 */
  tmr_channel_value_set(TMR4, TMR_SELECT_CHANNEL_3,0);



}


/**
 * @brief  设置 TMR4 通道 3 (DIM_PWM) 的占空比
 * @param  duty_percent: 占空比百分比 (0 ~ 100)
 * @retval None
 */
void wk_tmr4_ch3_set_duty(uint8_t duty_percent)
{
  uint32_t pulse_value = 0;

  /* 限制输入范围在 0 ~ 100 之间 */
  if (duty_percent > 100)
  {
    duty_percent = 100;
  }

  /* 根据 ARR = 59999 计算对应的比较值 (Period = 59999, 实际总计数点位 = 60000) */
  pulse_value = (uint32_t)(60000 * duty_percent) / 100;

  /* 如果要输出严格的 50%，pulse_value 即为 30000 */
  tmr_channel_value_set(TMR4, TMR_SELECT_CHANNEL_3, pulse_value);
}

