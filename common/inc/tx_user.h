#ifndef TX_USER_H
#define TX_USER_H

/* 1. 关闭参数错误检查，大幅减小代码体积 */
#define TX_DISABLE_ERROR_CHECKING

/* 2. 让定时器直接在中断中处理，省去定时器线程开销 */
#define TX_TIMER_PROCESS_IN_ISR

#endif


