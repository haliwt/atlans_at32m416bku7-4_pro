#include "bsp.h"

care_machine_status_t g_care_status;

/* 初始化全局状态变量 (默认为开机就绪状态) */
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

/* 硬件底层 CAN FD 发送接口伪代码，对接 AT32 硬件时需根据标准库实现 */
extern void AT32_CANFD_SendData(uint32_t id, uint8_t *p_data, uint8_t len);

/**
  * @brief  计算和校验码 (CS)
  * @param  p_buf: 数据缓冲区指针
  * @param  len: 计算长度 (从帧头到功能码)
  * @retval 1字节校验和
  */
static uint8_t calculate_checksum(uint8_t *p_buf, uint8_t len)
{
    uint32_t sum = 0;
    for (uint8_t i = 0; i < len; i++)
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
    /* 1. 基础长度与帧头帧尾校验 */
    if (len < 8 || p_data[0] != FRAME_HEADER || p_data[7] != FRAME_TAIL)
    {
        return; 
    }

    /* 2. 接收地址校验 (本设备目标地址固定为 0x80) */
    uint16_t rx_addr = (p_data[1] << 8) | p_data[2];
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
    uint8_t cal_cs = calculate_checksum(p_data, 6);
    if (cal_cs != p_data[6])
    {
        return; // 校验错误，丢弃
    }

    /* 5. 协议指令业务逻辑解析 */
    uint8_t func_code = p_data[4];
    uint8_t gear_param = p_data[5];

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
            if (g_care_status.power_status == 1)
            {
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
                    else
                    {
                        g_care_status.remain_seconds = 1800; // 默认 30 分钟 (1800s)
                    }
                }
            }
            break;

        default:
            break;
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
        tx_buf[2] = 0x0E;
        tx_buf[3] = 0x07;

        /* 2. 组装 Byte 0: 功能状态 (过程型 Bit 0~1 | 干燥型 Bit 2) */
        tx_buf[4] = (g_care_status.process_func & 0x03);
        if (g_care_status.dry_func_en)
        {
            tx_buf[4] |= (1 << 2); // Bit 2 置 1
        }

        /* 3. 组装 Byte 1: 挡位状态 (过程型挡位 Bit 0~1 | 干燥型挡位 Bit 2~3) */
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

        /* 10. 维持线程 500ms 周期（假设 1 Tick = 1ms） */
        tx_thread_sleep(500);
    }
}







