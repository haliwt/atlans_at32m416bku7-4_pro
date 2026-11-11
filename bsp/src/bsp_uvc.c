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

