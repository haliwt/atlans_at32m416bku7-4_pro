#include "bsp.h"


_main_pro_t  gpro_t ;

//static void care_machine_init(void);
void bsp_init(void)
{
 

}




/**
 * @brief  设置并启动指定功能及档位
 * @param  func: 功能选择 (干燥、除异味、除螨、消毒)
 * @param  gear: 档位选择 (GEAR_1:30min, GEAR_2:45min, GEAR_3:60min)
 */
void care_machine_set_function(care_func_t func, care_gear_t gear)
{
    /* 1. 如果是设定维持型功能：干燥 */
    if (func == FUNC_DRY)
    {
        gpro_t.dry_gear = gear;
        gpro_t.is_dry_active = true;
       
    }

    /* 2. 过程型功能互斥：同一时间只允许一个过程型功能运行 */
    gpro_t.active_process_func = func;
    gpro_t.process_gear = gear;
    gpro_t.is_process_running = true;
    gpro_t.is_process_paused = false;

    /* 根据档位分配运行时长：1档=30分钟, 2档=45分钟, 3档=60分钟 */
    switch (gear)
    {

        case GEAR_0 :
		   gpro_t.process_countdown_s =0;
       break;
		
		case GEAR_1:
            gpro_t.process_countdown_s = 30 * 60;
            break;
        case GEAR_2:
            gpro_t.process_countdown_s = 45 * 60;
            break;
        case GEAR_3:
            gpro_t.process_countdown_s = 60 * 60;
            break;
        default:
            gpro_t.process_countdown_s = 45 * 60;
            break;
    }
}


