#include "bsp.h"

//care_machine_status_t g_care_status;


/* 初始化全局状态变量 (默认为开机就绪状态) */
#if 0
care_machine_status_t g_care_status = {
    .power_status = 1,
    .process_func = 0,
    .dry_func_en = false,
    .process_gear = 0,
    .dry_gear = 0,
    .remain_seconds = 0,
    .temperature = 40,          // 默认示例温度
    .humidity = 30,             // 默认示例湿度
    .fault_code = FAULT_NONE
};
#else 
/* 2. 严格按照结构体成员顺序依次赋值（传统 C89 语法） */
care_machine_status_t g_care_status = {
    1,          // power_status
    0,          // process_func
    false,      // dry_func_en
    0,          // process_gear
    0,          // dry_gear
    0,          // remain_seconds
    40,         // temperature
    30,         // humidity
    FAULT_NONE  // fault_code
};


#endif 


/**
  * @brief  计算和校验码 (CS)
  * @param  p_buf: 数据缓冲区指针
  * @param  len: 计算长度 (从帧头到功能码)
  * @retval 1字节校验和
  */
static uint8_t calculate_checksum(uint8_t *p_buf, uint8_t len)
{
    uint32_t sum = 0;
	uint8_t i;
    for (i = 0; i < len; i++)
    {
        sum += p_buf[i];
    }
    return (uint8_t)(sum & 0xFF);
}

/**
  * @brief  解析接收到的 CAN FD 控制数据帧 (下发控制协议)
  * @param  p_data: 接收到的 CAN 数据域指针
  * @param  len: 数据域长度
  * @retval 无
  */
void protocol_parse_ctrl_frame(uint8_t *p_data, uint8_t len)
{
     uint16_t rx_addr;
	uint8_t cal_cs,func_code ,gear_param;
	
	/* 1. 基础长度与帧头帧尾校验 */
    if (len < 8 || p_data[0] != FRAME_HEADER || p_data[7] != FRAME_TAIL)
    {
        return; 
    }

    /* 2. 接收地址校验 (本设备目标地址固定为 0x80) */
    rx_addr = (p_data[1] << 8) | p_data[2];
    if (rx_addr != 0x0080)
    {
        return; 
    }

    /* 3. 功能码长度校验 (固定为 0x02) */
    if (p_data[3] != 0x02)
    {
        return;
    }

    /* 4. 和校验验证 (计算 Byte 0 到 Byte 5 的和) */
    cal_cs = calculate_checksum(p_data, 6);
    if (cal_cs != p_data[6])
    {
        return; // 校验错误，丢弃
    }

    /* 5. 协议指令业务逻辑解析 */
     func_code = p_data[4];
    gear_param = p_data[5];

    switch (func_code)
    {
        case FUNC_POWER: // 开关机控制
            if (gear_param == 0x00)      // 关机
            {
                g_care_status.power_status = 0;
                g_care_status.process_func = 0;
                g_care_status.dry_func_en = false;
                g_care_status.remain_seconds = 0;
            }
            else if (gear_param == 0x04) // 开机
            {
                g_care_status.power_status = 1;
            }
            break;

        case FUNC_DRY: // 干燥控制 (可与过程型并行)
            if (g_care_status.power_status == 1)
            {
                if (gear_param == 0x00)
                {
                    g_care_status.dry_func_en = false;
                    g_care_status.dry_gear = 0;
                }
                else if (gear_param >= 0x01 && gear_param <= 0x03)
                {
                    g_care_status.dry_func_en = true;
                    g_care_status.dry_gear = gear_param;
                }
            }
            break;

        case FUNC_DEODORIZE: // 除异味
        case FUNC_MITES:     // 除螨
        case FUNC_DISINFECT: // 消毒
            //if (g_care_status.power_status == 1)
            //{
                if (gear_param == 0x00) // 关闭当前过程型功能
                {
                    g_care_status.process_func = 0;
                    g_care_status.process_gear = 0;
                    g_care_status.remain_seconds = 0;
                }
                else if (gear_param >= 0x01 && gear_param <= 0x03)
                {
                    g_care_status.process_func = func_code; // 对应 0x01, 0x02, 0x03
                    g_care_status.process_gear = gear_param;
                    
                    // 根据协议设定动态初始化剩余时间 (以消毒为例)
                    if (func_code == FUNC_DISINFECT)
                    {
                        if (gear_param == 1) g_care_status.remain_seconds = 30 * 60;
                        else if (gear_param == 2) g_care_status.remain_seconds = 45 * 60;
                        else if (gear_param == 3) g_care_status.remain_seconds = 60 * 60;
                    }
					else if (func_code == FUNC_DEODORIZE) //除异味
                    {
                        if (gear_param == 1) g_care_status.remain_seconds = 30 * 60;
                        else if (gear_param == 2) g_care_status.remain_seconds = 45 * 60;
                        else if (gear_param == 3) g_care_status.remain_seconds = 60 * 60;
                    }
					else if (func_code == FUNC_MITES) //除螨虫
                    {
                        if (gear_param == 1) g_care_status.remain_seconds = 30 * 60;
                        else if (gear_param == 2) g_care_status.remain_seconds = 45 * 60;
                        else if (gear_param == 3) g_care_status.remain_seconds = 60 * 60;
                    }
					else if (func_code == FUNC_DRY) //除异味
                    {
                        if (gear_param == 1) g_care_status.remain_seconds = 30 * 60;
                        else if (gear_param == 2) g_care_status.remain_seconds = 45 * 60;
                        else if (gear_param == 3) g_care_status.remain_seconds = 60 * 60;
                    }
                    else
                    {
                        g_care_status.remain_seconds = 0; // 默认 0
                    }
                }
           // }
            break;

        default:
            break;
    }
}

/**
  * @brief  ThreadX 独立的 CAN FD 协议接收与业务解析任务 (线程)
  * @note   无数据时休眠，有数据时被激活，保证极致的实时响应与低功耗
  */
void can_parse_thread_entry(ULONG thread_input)
{
    while (1)
    {
        /* 1. 永久等待信号量（TX_WAIT_FOREVER），无报文时线程处于挂起状态，不占用 CPU */
        if (tx_semaphore_get(&can_rx_semaphore, TX_WAIT_FOREVER) == TX_SUCCESS)
        {
            /* 2. 成功被中断唤醒，提取全局缓冲区 g_can_rx_buffer 中的硬件数据进行解析 */
            /* 转换枚举类型为具体的字节长度 */
            uint8_t frame_len = (uint8_t)g_can_rx_buffer.data_length; 
            
            /* 调用业务层协议解析函数 */
            protocol_parse_ctrl_frame(g_can_rx_buffer.data, frame_len);
        }
    }
}

/**
  * @brief  ThreadX 500ms 周期状态广播发送线程
  * @param  thread_input: 线程输入参数
  * @retval 无
  */
void broadcast_thread_entry(ULONG thread_input)
{
    uint8_t tx_buf[13]; // 帧头(1) + 广播地址(2) + 长度(1) + 数据域(7) + 校验(1) + 帧尾(1) = 13 字节

    while (1)
    {
        /* 1. 组装固定帧头及地址（广播地址 0x000E，全量包长度 0x07） */
        tx_buf[0] = FRAME_HEADER;
        tx_buf[1] = 0x00;
        tx_buf[2] = 0x0E;// 接收广播地址[cite: 5]
        tx_buf[3] = 0x07;// 数据长度[cite: 5]

        /* 2. 组装 Byte 0: 功能状态 (过程型 Bit 0~1 | 干燥型 Bit 2) */
        tx_buf[4] = (g_care_status.process_func & 0x03);
        if (g_care_status.dry_func_en)
        {
            tx_buf[4] |= (1 << 2); // Bit 2 置 1
        }

        /* 3. 组装 Byte 1: 挡位状态 (过程型挡位 Bit 0~1 | 干燥型挡位 Bit 2~3)(gear-1,-2,-3) */
        tx_buf[5] = (g_care_status.process_gear & 0x03);
        tx_buf[5] |= ((g_care_status.dry_gear & 0x03) << 2);

        /* 4. 组装 Byte 2~3: 剩余时间 (大端模式) */
        tx_buf[6] = (uint8_t)((g_care_status.remain_seconds >> 8) & 0xFF);
        tx_buf[7] = (uint8_t)(g_care_status.remain_seconds & 0xFF);

        /* 5. 组装 Byte 4 & Byte 5: 温湿度 */
        tx_buf[8] = g_care_status.temperature;
        tx_buf[9] = g_care_status.humidity;

        /* 6. 组装 Byte 6: 故障码 */
        tx_buf[10] = g_care_status.fault_code;

        /* 7. 计算和校验 (从 tx_buf[0] 加到 tx_buf[10]) */
        tx_buf[11] = calculate_checksum(tx_buf, 11);

        /* 8. 填充结束帧 */
        tx_buf[12] = FRAME_TAIL;

        /* 9. 调用底层硬件驱动，通过扩展帧发送全量 CAN FD 报文 */
        AT32_CANFD_SendData(CAN_ID_BROADCAST, tx_buf, 13);

        /* 10. 维持线程 500ms 周期（假设 1 Tick = 10ms） */
        tx_thread_sleep(50);
    }
}


#if 0
/**
  * @brief  雅特力 AT32M416 业务级 CAN FD 报文发送函数（基于 2025 官方固件库）
  * @param  id:     发送的 CAN ID (11位标准ID 或 29位扩展ID)
  * @param  p_data: 指向待发送数据的指针
  * @param  len:    发送数据字节长度 (数据流长度，如广播包传 13)
  * @retval error_status: 返回 SUCCESS (0) 发送成功, 或 ERROR (1) 发送失败/总线满
  */
#if 0
error_status AT32_CANFD_SendData(uint32_t id, uint8_t *p_data, uint8_t len)
{
    /* --- ARMCC V5 铁律：所有局部变量必须定义在函数体最顶部 --- */
    can_txbuf_type tx_struct;
    error_status status;
    uint32_t timeout_counter;

    /* 1. 基础指针非空及长度合法性校验 */
    if ((p_data == NULL) || (len == 0))
    {
        return ERROR;
    }

    /* 2. 结构体初始化，清除栈内存随机值 */
    tx_struct.id = id;
    tx_struct.data = p_data;        // 官方数组成员名确认为 'data' (对应源码中的 rp = can_txbuf_struct->data)
    tx_struct.tx_timestamp = 0;
    tx_struct.handle = 0;

    /* 3. 自动根据 ID 大小选择帧格式 */
    if (id > 0x7FF)
    {
        tx_struct.id_type = CAN_ID_EXTENDED;  // 29位扩展帧
    }
    else
    {
        tx_struct.id_type = CAN_ID_STANDARD;  // 11位标准帧
    }

    /* 4. 配置为标准数据帧 */
    tx_struct.frame_type = CAN_FRAME_DATA;   // 数据帧

    /* 5. 处理数据长度 (完美匹配官方库规范) */
#ifdef SUPPORT_CAN_FD
    /* 针对 CAN FD：如果你传入的 len 是 13，需要对应到硬件的 dlc 编码（13字节需要向上对齐到16字节的DLC，即 0x0A） */
    /* 这里的 data_length 对应官方驱动代码中 tbfmt_bit.dlc 的赋值 */
    if (len <= 8)  tx_struct.data_length = len;
    else if (len <= 12) tx_struct.data_length = 9;   // DLC = 9  -> 12 字节
    else if (len <= 16) tx_struct.data_length = 10;  // DLC = 10 -> 16 字节（你传入13字节，硬件按16字节FD包发出）
    else if (len <= 20) tx_struct.data_length = 11;  // DLC = 11 -> 20 字节
    else if (len <= 24) tx_struct.data_length = 12;  // DLC = 12 -> 24 字节
    else if (len <= 32) tx_struct.data_length = 13;  // DLC = 13 -> 32 字节
    else if (len <= 48) tx_struct.data_length = 14;  // DLC = 14 -> 48 字节
    else tx_struct.data_length = 15;                 // DLC = 15 -> 64 字节

    tx_struct.fd_format = TRUE;              // 对应官方 tbfmt_bit.fdf
    tx_struct.fd_rate_switch = TRUE;         // 对应官方 tbfmt_bit.brs，开启波特率切换提升速率
#else
    /* 传统 CAN 模式 */
    tx_struct.data_length = (len > 8) ? 8 : len;
#endif

    /* 6. 第一步：调用官方底层将数据装填进最高优先级硬件缓冲区 (PTB) */
    status = can_txbuf_write(CAN1, CAN_TXBUF_PTB, &tx_struct);
    if (status == ERROR)
    {
        /* 硬件 PTB 缓冲区可能被上一次未发送完的报文锁死，返回错误让上层重新调度 */
        return ERROR; 
    }

    /* 7. 第二步：调用官方底层触发发送指令（临门一脚） */
    status = can_txbuf_transmit(CAN1, CAN_TRANSMIT_PTB);
    if (status == ERROR)
    {
        return ERROR;
    }

    /* 8. 闭环防爆舱设计：等待发送结束（防止大吞吐量连续调用顶爆缓冲区） */
    timeout_counter = 0;
    /* 监控硬件寄存器中的 TPE (Primary Transmit Blocked/Busy) 状态 */
    while (CAN1->ctrlstat_bit.tpe == TRUE)
    {
        timeout_counter++;
        /* 设定安全超时计数。500ms 周期任务绝不能卡死在死循环中 */
        if (timeout_counter > 60000) 
        {
            return ERROR; // 发送超时，总线可能断开或无终端电阻，退出防止看门狗复位
        }
    }

    return SUCCESS; // 成功发送
}

#else 
/**
 * @brief  雅特力 AT32M416 业务级 CAN FD 报文发送函数（修正版）
 * @param  id:       发送的 CAN ID (11位标准ID 或 29位扩展ID)
 * @param  p_data:   指向待发送数据的指针
 * @param  len:      发送数据字节长度 (数据流长度，如广播包传 13)
 * @retval error_status: 返回 SUCCESS (0) 发送成功, 或 ERROR (1) 发送失败/总线忙
 */
error_status AT32_CANFD_SendData(uint32_t id, uint8_t *p_data, uint8_t len)
{
    can_txbuf_type tx_struct;
    error_status status;
    uint32_t timeout_counter;

    /* 1. 基础指针非空及长度合法性校验 */
    if ((p_data == NULL) || (len == 0))
    {
        return ERROR;
    }

    /* 2. 结构体初始化，清除栈内存随机值 */
    tx_struct.id = id;
    tx_struct.data = p_data;
    tx_struct.tx_timestamp = 0;
    tx_struct.handle = 0;

    /* 3. 自动根据 ID 大小选择帧格式 */
    if (id > 0x7FF)
    {
        tx_struct.id_type = CAN_ID_EXTENDED;  // 29位扩展帧
    }
    else
    {
        tx_struct.id_type = CAN_ID_STANDARD;  // 11位标准帧
    }

    /* 4. 配置为标准数据帧 */
    tx_struct.frame_type =  CAN_FRAME_DATA;   // 数据帧

    /* 5. 处理数据长度 (完美匹配官方库规范) */
#ifdef SUPPORT_CAN_FD
    if (len <= 8)         tx_struct.data_length = len;
    else if (len <= 12)   tx_struct.data_length = 9;   // 12 字节
    else if (len <= 16)   tx_struct.data_length = 10;  // 16 字节
    else if (len <= 20)   tx_struct.data_length = 11;  // 20 字节
    else if (len <= 24)   tx_struct.data_length = 12;  // 24 字节
    else if (len <= 32)   tx_struct.data_length = 13;  // 32 字节
    else if (len <= 48)   tx_struct.data_length = 14;  // 48 字节
    else                  tx_struct.data_length = 15;  // 64 字节

    tx_struct.fd_format = TRUE;                 // 开启 CAN FD 格式
    tx_struct.fd_rate_switch = TRUE;            // 开启 BRS 速率切换
#else
    tx_struct.data_length = (len > 8) ? 8 : len;
#endif

    /* 6. 第一步：调用官方底层将数据装填进最高优先级硬件缓冲区 (PTB) */
    status = can_txbuf_write(CAN1, CAN_TXBUF_PTB, &tx_struct);
    if (status == ERROR)
    {
        return ERROR; 
    }

    /* 7. 第二步：调用官方底层触发发送指令 */
    status = can_txbuf_transmit(CAN1, CAN_TRANSMIT_PTB);
    if (status == ERROR)
    {
        return ERROR;
    }

    /* 8. 闭环防爆舱设计：使用官方标准 API 等待发送结束标志 (TPIF) */
    timeout_counter = 0;
    while (can_flag_get(CAN1, CAN_TPIF_FLAG) != SET)
    {
        timeout_counter++;
        if (timeout_counter > 60000)  
        {
            return ERROR; // 发送超时，可能总线断开或无响应
        }
    }
    
    /* 清除发送完成标志，为下一次发送做准备 */
    can_flag_clear(CAN1, CAN_TPIF_FLAG);

    return SUCCESS; 
}




#endif 

#endif 

/**
 * @brief  雅特力 AT32M416 业务级 CAN FD 报文发送函数（高可靠无 Bug 版）
 * @param  id:       发送的 CAN ID (11位标准ID 或 29位扩展ID)
 * @param  p_data:   指向待发送数据的指针
 * @param  len:      发送数据字节长度 (数据流长度，如广播包传 13)
 * @retval error_status: 返回 SUCCESS (0) 发送成功, 或 ERROR (1) 发送失败/总线忙
 */
 #if 0
error_status AT32_CANFD_SendData(uint32_t id, uint8_t *p_data, uint8_t len)
{
    /* --- ARMCC V5 铁律：所有局部变量必须定义在函数体最顶部 --- */
    can_txbuf_type tx_struct;
    error_status status;
    uint32_t timeout_counter;

    /* 1. 基础指针非空及长度合法性校验 */
    if ((p_data == NULL) || (len == 0))
    {
        return ERROR;
    }

    /* 2. 结构体初始化，清除栈内存随机值 */
    tx_struct.id = id;
    tx_struct.data = p_data;
    tx_struct.tx_timestamp = 0;
    tx_struct.handle = 0;

    /* 3. 自动根据 ID 大小选择帧格式 */
    if (id > 0x7FF)
    {
        tx_struct.id_type = CAN_ID_EXTENDED;  // 29位扩展帧
    }
    else
    {
        tx_struct.id_type = CAN_ID_STANDARD;  // 11位标准帧
    }

    /* 4. 配置为标准数据帧 */
    tx_struct.frame_type = CAN_FRAME_DATA;   // 数据帧

    /* 5. 处理数据长度 (完美匹配官方库规范) */
#ifdef SUPPORT_CAN_FD
    if (len <= 8)         tx_struct.data_length = len;
    else if (len <= 12)   tx_struct.data_length = 9;   // 12 字节
    else if (len <= 16)   tx_struct.data_length = 10;  // 16 字节 (13字节广播包会自动落入此区间)
    else if (len <= 20)   tx_struct.data_length = 11;  // 20 字节
    else if (len <= 24)   tx_struct.data_length = 12;  // 24 字节
    else if (len <= 32)   tx_struct.data_length = 13;  // 32 字节
    else if (len <= 48)   tx_struct.data_length = 14;  // 48 字节
    else                  tx_struct.data_length = 15;  // 64 字节

    /* ⚠️ 修正：此处必须使用官方库标准定义的专用宏，否则会引发类型或值匹配警告 */
    tx_struct.fd_format = CAN_FORMAT_FD;         // 开启 CAN FD 格式
    tx_struct.fd_rate_switch = CAN_BRS_ON;       // 开启 BRS 速率切换
#else
    tx_struct.data_length = (len > 8) ? 8 : len;
#endif

    /* 6. 第一步：调用官方底层将数据装填进最高优先级硬件缓冲区 (PTB) */
    status = can_txbuf_write(CAN1, CAN_TXBUF_PTB, &tx_struct);
    if (status == ERROR)
    {
        return ERROR; 
    }

    /* 7. 第二步：调用官方底层触发发送指令 */
    status = can_txbuf_transmit(CAN1, CAN_TRANSMIT_PTB);
    if (status == ERROR)
    {
        return ERROR;
    }

    /* 8. 闭环防爆舱设计：使用官方标准 API 等待发送结束标志 (TPIF) */
    timeout_counter = 0;
    while (can_flag_get(CAN1, CAN_TPIF_FLAG) != SET)
    {
        timeout_counter++;
        if (timeout_counter > 60000)  
        {
            return ERROR; // 发送超时，可能总线断开或无响应
        }
    }
    
    /* 清除发送完成标志，为下一次发送做准备 */
    can_flag_clear(CAN1, CAN_TPIF_FLAG);

    return SUCCESS; 
}
#else 
error_status AT32_CANFD_SendData(uint32_t id, uint8_t *p_data, uint8_t len)
{
    /* --- ARMCC V5 铁律：所有局部变量必须定义在函数体最顶部 --- */
    can_txbuf_type tx_struct;
    error_status status;
    uint32_t timeout_counter;
    uint8_t i; // 新增：用于数据拷贝的循环变量

    /* 1. 基础指针非空及长度合法性校验 */
    if ((p_data == NULL) || (len == 0))
    {
        return ERROR;
    }

    /* 2. 结构体初始化，清除栈内存随机值 */
    tx_struct.id = id;
    tx_struct.tx_timestamp = 0;
    tx_struct.handle = 0;

    /* 💡 修复 #137 报错：不能直接对数组名赋值，必须逐字节拷贝数据 💡 */
    for (i = 0; i < len; i++)
    {
        tx_struct.data[i] = p_data[i];
    }

    /* 3. 自动根据 ID 大小选择帧格式 */
    if (id > 0x7FF)
    {
        tx_struct.id_type = CAN_ID_EXTENDED;  // 29位扩展帧
    }
    else
    {
        tx_struct.id_type = CAN_ID_STANDARD;  // 11位标准帧
    }

    /* 4. 配置为标准数据帧 */
    tx_struct.frame_type = CAN_FRAME_DATA;   // 数据帧

    /* 5. 处理数据长度 (完美匹配官方库规范) */
#ifdef SUPPORT_CAN_FD
    /* 采用官方标准 DLC 长度赋值 */
    if (len <= 8)         tx_struct.data_length = len;
    else if (len <= 12)   tx_struct.data_length = 9;   // 12 字节
    else if (len <= 16)   tx_struct.data_length = 10;  // 16 字节 (13字节广播包会自动落入此区间)
    else if (len <= 20)   tx_struct.data_length = 11;  // 20 字节
    else if (len <= 24)   tx_struct.data_length = 12;  // 24 字节
    else if (len <= 32)   tx_struct.data_length = 13;  // 32 字节
    else if (len <= 48)   tx_struct.data_length = 14;  // 48 字节
    else                  tx_struct.data_length = 15;  // 64 字节

    tx_struct.fd_format = CAN_FORMAT_FD;         // 开启 CAN FD 格式
    tx_struct.fd_rate_switch = CAN_BRS_ON;       // 开启 BRS 速率切换
#else
    tx_struct.data_length = (len > 8) ? 8 : len;
#endif

    /* 6. 第一步：调用官方底层将数据装填进最高优先级硬件缓冲区 (PTB) */
    status = can_txbuf_write(CAN1, CAN_TXBUF_PTB, &tx_struct);
    if (status == ERROR)
    {
        return ERROR; 
    }

    /* 7. 第二步：调用官方底层触发发送指令 */
    status = can_txbuf_transmit(CAN1, CAN_TRANSMIT_PTB);
    if (status == ERROR)
    {
        return ERROR;
    }

    /* 8. 闭环防爆舱设计：使用官方标准 API 等待发送结束标志 (TPIF) */
    timeout_counter = 0;
    while (can_flag_get(CAN1, CAN_TPIF_FLAG) != SET)
    {
        timeout_counter++;
        if (timeout_counter > 60000)  
        {
            return ERROR; // 发送超时，可能总线断开或无响应
        }
    }
    
    /* 清除发送完成标志，为下一次发送做准备 */
    can_flag_clear(CAN1, CAN_TPIF_FLAG);

    return SUCCESS; 
}





#endif 

