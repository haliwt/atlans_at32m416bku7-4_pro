#include "bsp.h"

/**
  * @brief	TMR1 触发/霍尔 与 TMR11 共享中断服务函数
  * @param	无
  * @retval 无
  */
void tmr11_irq(void)
{

	 /* add user code begin TMR1_TRG_HALL_TMR11_IRQ 0 */
	
	  /* 1. 检查是否是 TMR11 的溢出/更新中断触发 */
	  if(tmr_interrupt_flag_get(TMR11, TMR_OVF_FLAG) != RESET)
	  {
		  /* ==================== 10ms 用户业务逻辑代码 ==================== */
		  
		  // 静态变量计数，每进 100 次代表过去了 1 秒 (10ms * 100 = 1000ms)
		  static uint16_t timer_10ms_cnt = 0; 
		  
		  timer_10ms_cnt++;
		  if(timer_10ms_cnt >= 100)
		  {
			  timer_10ms_cnt = 0; // 1秒时间到，清零计数器
			  
			  /* 执行衣物护理机的秒倒计时逻辑 */
			  if(g_care_status.power_status == 1 && g_care_status.remain_dry_seconds > 0)
			  {
				  g_care_status.remain_dry_seconds--;
				  
				  /* 倒计时自然结束，按协议更新状态 */
				  if(g_care_status.remain_dry_seconds == 0)
				  {
					  g_care_status.process_func = 0; // 关闭过程型功能
					  g_care_status.process_gear = 0; // 挡位变为0挡提示结束
				  }
			  }
		  }
	
		  /* ============================================================= */
	
		  /* 2. 必须手动清除 TMR11 的中断标志位 */
		  tmr_flag_clear(TMR11, TMR_OVF_FLAG);
	  }
	
	  /* add user code end TMR1_TRG_HALL_TMR11_IRQ 0 */
	
	  /* add user code begin TMR1_TRG_HALL_TMR11_IRQ 1 */
	  
	  /* 注：如果你的项目后续需要用到 TMR1 的霍尔或触发中断，在这里用 else if 判断并处理 TMR1 */
	
	  /* add user code end TMR1_TRG_HALL_TMR11_IRQ 1 */
	
}

