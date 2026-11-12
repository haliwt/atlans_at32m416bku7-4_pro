#ifndef __BSP_CAN_PROTOCOL_H
#define __BSP_CAN_PROTOCOL_H 
#include <stdbool.h>

#include "at32m412_416_wk_config.h"
#include "tx_api.h"               
#include "at32m412_416_can.h"     

/* CAN FD ID 定义 */
#define CAN_ID_CTRL         0x18000080  // 控制命令接收 ID (扩展帧)
#define CAN_ID_BROADCAST    0x1800000E  // 状态广播发送 ID (扩展帧)

/* 帧固定字节 */
#define FRAME_HEADER        0xAA
#define FRAME_TAIL          0x55

/* 功能码定义 (Byte 0) */
#define FUNC_DEODORIZE      0x01        // 除异味
#define FUNC_MITES          0x02        // 除螨
#define FUNC_DISINFECT      0x03        // 消毒
#define FUNC_DRY            0x04        // 干燥
#define FUNC_POWER          0x05        // 开关机

/* 故障码定义 */
#define FAULT_NONE          0x00        // 无故障
#define FAULT_DOOR          0x01        // 门开关异常 (E1)
#define FAULT_SENSOR        0x02        // 传感器异常 (E2)
#define FAULT_OVERTEMP      0x03        // 超温保护
#define FAULT_DOOR_OPEN     0xFF        // 门开暂停

/* 护理机内部运行状态结构体 */
typedef struct {
    uint8_t  power_status;      // 0: 关机/待机, 1: 开机/就绪
    bool  power_disingfect_status ;
	bool  power_deodorize_status ;
	bool  power_mites_status ;
	bool  power_dry_status;
	
    uint8_t  process_func;      // 过程型功能 (0:关, 1:除异味, 2:除螨, 3:消毒)
    bool     dry_func_en;       // 干燥功能开关 (false:关, true:开)
    uint8_t  process_gear;      // 过程型功能挡位 (0:关,0x01-gear ,0x02-gear,0x03-gear)
    uint8_t  dry_gear;          // 干燥功能挡位 (0~3)
    uint8_t  deodorize_gear;
	uint8_t  mites_gear;
	uint8_t  disingfect_gear;
    
    uint8_t   process_deodorize_fan_speed ;
	uint8_t   process_mites_fan_speed ;
	uint8_t   process_disingfect_fan_speed ;
	uint8_t   process_dry_fan_speed ;

	
    uint16_t remain_deodorize_seconds;    // 剩余运行时间 (单位: 秒)
    uint16_t remain_mites_seconds;    // 剩余运行时间 (单位: 秒)
    uint16_t remain_disingfect_seconds;    // 剩余运行时间 (单位: 秒)
    uint16_t remain_dry_seconds;    // 剩余运行时间 (单位: 秒)
    uint8_t  temperature;       // 实时温度 (℃)
    uint8_t  humidity;          // 实时湿度 (%RH)
    uint8_t  fault_code;        // 当前故障码
    bool  done_task_deodorize_flag;
	bool  done_task_mites_flag;
	bool  done_task_disinfect_flag;
	 bool  done_task_dry_flag;
} care_machine_status_t;

/* 全局全局状态变量声明 */
extern care_machine_status_t g_care_status;

/* 异步通信组件声明 */
extern TX_SEMAPHORE   can_rx_semaphore; 
extern can_rxbuf_type g_can_rx_buffer; 

/* 函数声明 */
void protocol_parse_ctrl_frame(uint8_t *p_data, uint8_t len);
void can_parse_thread_entry(ULONG thread_input); // 新增解析线程
void broadcast_thread_entry(ULONG thread_input);

/* 硬件底层 CAN FD 发送接口伪代码，对接 AT32 硬件时需根据标准库实现 */
error_status AT32_CANFD_SendData(uint32_t id, uint8_t *p_data, uint8_t len);


#endif /* __BSP_CAN_PROTOCOL_H */


