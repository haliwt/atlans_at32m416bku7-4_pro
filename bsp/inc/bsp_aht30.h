#ifndef __BSP_AHT30_H  
#define __BSP_AHT30_H
#include "at32m412_416_wk_config.h"


#define I2C_SCL_GPIO_PORT     GPIOB
#define I2C_SCL_GPIO_PIN     GPIO_PINS_5

#define I2C_SDA_GPIO_PORT     GPIOB
#define I2C_SDA_GPIO_PIN      GPIO_PINS_6


void AHT30_Init(void);

uint8_t AHT30_Read_Data(int16_t *temperature, int16_t *humidity);




#endif 





