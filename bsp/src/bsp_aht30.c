#include "bsp.h"



/* 微秒级延时函数（基于 180MHz 主频或你当前系统的粗略延时，可根据实际适当调整） */
static void delay_us(uint32_t us)
{
    volatile uint32_t i;
	while(us--){
    for (i = 0; i < 180; i++) {//45
        __NOP();
    }
	}
}

/* 毫秒级延时函数 */
static void delay_ms(uint32_t ms)
{
     uint16_t i;
	for ( i = 0; i < ms; i++) {
        delay_us(1000);
    }
}

/* 配置 SDA 引脚为输出模式 */
static void SDA_OUT_MODE(void)
{
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    gpio_init_struct.gpio_pins = I2C_SDA_GPIO_PIN;
    gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN; // 开漏输出配合外接上拉
    gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  
    gpio_init(I2C_SDA_GPIO_PORT, &gpio_init_struct);
}

/* 配置 SDA 引脚为输入模式 */
static void SDA_IN_MODE(void)
{
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    gpio_init_struct.gpio_pins = I2C_SDA_GPIO_PIN;
    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_pull = GPIO_PULL_UP;
    gpio_init(I2C_SDA_GPIO_PORT, &gpio_init_struct);
}

/* SCL 和 SDA 基础控制宏 */
#define SCL_HIGH()  gpio_bits_set(I2C_SCL_GPIO_PORT, I2C_SCL_GPIO_PIN)
#define SCL_LOW()   gpio_bits_reset(I2C_SCL_GPIO_PORT, I2C_SCL_GPIO_PIN)

#define SDA_HIGH()  gpio_bits_set(I2C_SDA_GPIO_PORT, I2C_SDA_GPIO_PIN)
#define SDA_LOW()   gpio_bits_reset(I2C_SDA_GPIO_PORT, I2C_SDA_GPIO_PIN)

#define READ_SDA()  ((gpio_input_data_bit_read(I2C_SDA_GPIO_PORT, I2C_SDA_GPIO_PIN) == RESET) ? 0 : 1)

/* I²C 起始信号 */
static void I2C_Start(void)
{
    SDA_HIGH();
    SCL_HIGH();
    delay_us(10);
    SDA_LOW();
    delay_us(10);
    SCL_LOW();
    delay_us(10);
}

/* I²C 停止信号 */
static void I2C_Stop(void)
{
  
    SDA_LOW();
     delay_us(10);
    SCL_HIGH();
     delay_us(10);
    SDA_HIGH();
     delay_us(10);
  
}

/* 等待 ACK 响应 */
static uint8_t I2C_Wait_Ack(void)
{
  
    uint8_t ucErrTime = 0;
   
    SDA_HIGH();
    delay_us(10);
    SCL_HIGH();
    delay_us(10);
    while (READ_SDA()) {
        ucErrTime++;
        if (ucErrTime > 250) {
            I2C_Stop();
            return 1; // ACK 失败
        }
    }
    SCL_LOW();
	delay_us(10);
    return 0; // ACK 成功
 
}

/* 发送一个字节 */
static void I2C_Send_Byte(uint8_t dat)
{
   uint8_t i;
    for (i = 0; i < 8; i++)
    {
        if (dat & 0x80)
            SDA_HIGH();
        else
            SDA_LOW();
        dat <<= 1;
        delay_us(10);//I2C_Delay();
        SCL_HIGH();
        delay_us(10);//I2C_Delay();
        SCL_LOW();
        delay_us(10);//I2C_Delay();
    }
}

/* 读取一个字节 */
static uint8_t I2C_Read_Byte(unsigned char ack)
{
    uint8_t i, dat = 0;
    SDA_HIGH(); // 释放SDA，准备读取
    for (i = 0; i < 8; i++)
    {
        dat <<= 1;
        SCL_HIGH();
        delay_us(10);//I2C_Delay();
        if (READ_SDA())
        {
            dat |= 0x01;
        }
        SCL_LOW();
        delay_us(10);//I2C_Delay();
    }
    
    if (ack)
        SDA_LOW();  // 发送 ACK
    else
        SDA_HIGH(); // 发送 NACK
    
    delay_us(10);//I2C_Delay();
    SCL_HIGH();
    delay_us(10);//I2C_Delay();
    SCL_LOW();
    delay_us(10);//I2C_Delay();
    SDA_HIGH(); // 释放SDA
    return dat;
}

/* AHT30 提供的 CRC8 校验函数 */
static uint8_t Calc_CRC8(uint8_t *message, uint8_t Num)
{
    uint8_t i;
    uint8_t byte;
    uint8_t crc = 0xFF;
    for (byte = 0; byte < Num; byte++) {
        crc ^= (message[byte]);
        for (i = 8; i > 0; --i) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31;
            } else {
                crc = (crc << 1);
            }
        }
    }
    return crc;
}

/* 初始化 GPIO */
void AHT30_Init(void)
{
    
	gpio_init_type gpio_init_struct;
	gpio_default_para_init(&gpio_init_struct);
	
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);


    // 初始化 SCL (PB5) 和 SDA (PB6) 为开漏输出配合内部/外部上拉
    gpio_init_struct.gpio_pins = I2C_SCL_GPIO_PIN | I2C_SDA_GPIO_PIN;
    gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
    gpio_init_struct.gpio_pull = GPIO_PULL_UP;
    gpio_init(GPIOB, &gpio_init_struct);

    SCL_HIGH();
    SDA_HIGH();
    
    delay_ms(10); // 上电等待稳定
}

/* 核心读取函数：返回 0 表示成功，返回 1 表示失败 */
uint8_t data[7];

uint8_t AHT30_Read_Data(int16_t *temperature, int16_t *humidity)
{
    uint32_t raw_humidity;
	uint32_t raw_temperature ;
	uint8_t crc_val;
	int16_t temp_value, hum_value;
	//uint8_t data[7];

	    // 【位置 1】在函数开头声明 ThreadX 中断状态保存变量
    TX_INTERRUPT_SAVE_AREA
    
    // 1. 发送触发测量命令：0x70 (写地址) -> 0xAC (触发) -> 0x33 -> 0x00[cite: 12]
    I2C_Start();
    I2C_Send_Byte(0x70); 
    if (I2C_Wait_Ack()) { I2C_Stop(); return 0xF0; }
    
    I2C_Send_Byte(0xAC); 
    if (I2C_Wait_Ack()) { I2C_Stop(); return 0XF1; }
    
    I2C_Send_Byte(0x33); 
    if (I2C_Wait_Ack()) { I2C_Stop(); return 0XF2; }
    
    I2C_Send_Byte(0x00); 
    if (I2C_Wait_Ack()) { I2C_Stop(); return 0XF3; }
    
    I2C_Stop();

    // 2. 等待传感器转换完成（手册要求至少等待 80ms）[cite: 12, 14]
    tx_thread_sleep(8) ;//delay_ms(80);

	// 【位置 2】开始连续读取前，关闭中断防止模拟 I2C 时序被打断
    TX_DISABLE;

    // 3. 读取 7 个字节数据：[Status][SRH_H][SRH_M][SRH_L][ST_H][ST_M][ST_L][CRC] -> 实际带 CRC 共 7 字节[cite: 12]
    I2C_Start();
    I2C_Send_Byte(0x71); // 读地址[cite: 12]
    if (I2C_Wait_Ack()){ 
		I2C_Stop(); 
		TX_RESTORE; // 提前返回前必须恢复中断！
	    return 1; 
	}

    // 检查忙闲状态（可选）：如果 Status 的 bit7 为 1 表示仍在测量
    data[0] = I2C_Read_Byte(1); // Status
    if (data[0] & 0x80) {       // 如果忙，说明 80ms 不够，直接返回失败
        I2C_Stop();
		TX_RESTORE; // 提前返回前必须恢复中断！
        return 2;
    }

    data[1] = I2C_Read_Byte(1); // 湿度高 8 位
    data[2] = I2C_Read_Byte(1); // 湿度中 8 位
    data[3] = I2C_Read_Byte(1); // 湿度低 4 位 + 温度高 4 位
    data[4] = I2C_Read_Byte(1); // 温度中 8 位
    data[5] = I2C_Read_Byte(1); // 温度低 8 位
    data[6] = I2C_Read_Byte(0); // CRC 校验字（最后一个字节发 NACK）[cite: 12]
    //crc_val = I2C_Read_Byte(0); // 最后的 CRC 字节，发送 NACK 结束[cite: 1]
    I2C_Stop();

	// 【位置 3】总线释放后，立刻恢复系统中断
    TX_RESTORE;

	/* 4. 检查状态字的 Bit[7] 是否为忙状态（1表示忙）[cite: 1] */
    if (data[0] & 0x80)
    {
        return 3; 
    }

    // 4. CRC8 校验检查
    if (Calc_CRC8(data, 6) != data[6]) {
        return 4; // 校验失败
    }

    // 5. 数据转换（按照 AHT30 手册公式转为整型或扩大 10 倍以避免浮点运算）[cite: 14]
    // 20位湿度原始值合成
    raw_humidity = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);
    // 20位温度原始值合成
     raw_temperature = (((uint32_t)data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];

    /* 
     * 遵循你偏好的整数运算（int16_t）以减少计算开销：
     * 湿度公式: RH% = (raw_humidity / 2^20) * 100 
     * 温度公式: T(℃) = (raw_temperature / 2^20) * 200 - 50 
     * 为保留一位小数，将结果扩大 10 倍存储：
     */
    #if 1
    hum_value = (int16_t)((raw_humidity * 1000) >> 20);           // 实际湿度 * 10 
    temp_value = (int16_t)(((raw_temperature * 2000) >> 20) - 500); // 实际温度 *10


	*humidity = hum_value / 10 ;

	*temperature = temp_value /10 ;

	#else 
	/* 
     * 7. 依据 AHT30 官方转换公式变换为整数[cite: 1]：
     * 湿度公式: RH% = (raw_hum / 2^20) * 100[cite: 1]
     * 温度公式: T℃  = (raw_temp / 2^20) * 200 - 50[cite: 1]
     * 
     * 注：此处加上 524288（即 2^20 的一半）是为了实现整数除法的“四舍五入”效果
     */
    *humidity = (int16_t)(((uint64_t)raw_humidity * 100 + 524288) / 1048576); 
    *temperature = (int16_t)(((int64_t)raw_temperature * 200 + 524288) / 1048576) - 50;



	#endif 

	

    return 0; // 读取成功
}



