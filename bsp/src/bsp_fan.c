#include "bsp.h"


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


