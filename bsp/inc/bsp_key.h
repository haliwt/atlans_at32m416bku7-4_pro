#ifndef __BSP_KEY_H
#define __BSP_KEY_H
#include "at32m412_416_wk_config.h"


#define KEY_POWER_VALUE()        gpio_input_data_bit_read(KEY_POWER_GPIO_PORT,KEY_POWER_GPIO_PIN)

#define KEY_MODE_VALUE()         gpio_input_data_bit_read(KEY_MODE_GPIO_PORT,KEY_MODE_GPIO_PIN)


#endif 


