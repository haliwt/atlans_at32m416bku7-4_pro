#include "bsp.h"


void CAN1_RX_IRQHandler(void)
{
    //can_rx_message_type rx_message;
   // uint8_t frame_len ;
    can_rxbuf_type rx_message;

    /* 1. 检查是否是接收中断 */
    if(can_interrupt_flag_get(CAN1, CAN_RIF_FLAG) != RESET) ///*!< receiver interrupt flag */
    {
        /* 2. 从硬件缓冲区中读取 CAN FD 报文 */
        //can_message_receive(CAN1, CAN_RX_FIFO0, &rx_message);
        //can_message_rx(CAN1, CAN_RX_FIFO0, &rx_message); 
        can_rxbuf_read(CAN1, &rx_message);

        /* 3. 过滤协议条件：必须是扩展帧，且 ID 为协议规定的 0x18000080 */
        if((rx_message.id_type == CAN_ID_EXTENDED) && (rx_message.id == CAN_ID_CTRL))
        {


             /* 4. 关键动作：向 ThreadX 释放一个信号量，立刻激活后台的解析线程 */
              tx_semaphore_put(&can_rx_semaphore);
           #if 0
                 /* 提取安全的字节长度，确保传参类型完全匹配 */
               frame_len = (uint8_t)rx_message.data_length;

		     /* 4. 将读取到的硬件数据，直接丢进我们写好的协议解析函数中 */
            /* 修正：将 rx_message.dtl 修改为官方结构体中的真实成员变量 data_length */
            protocol_parse_ctrl_frame(rx_message.data,frame_len);

			#endif 
        }

        /* 5. 关键修正：使用官方正确的 CAN_RIF_FLAG 清除接收中断标志，防止死循环 */
        can_flag_clear(CAN1, CAN_RIF_FLAG); 
    }
}




