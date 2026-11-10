/**
  **************************************************************************
  * @file     wk_system.h
  * @brief    workbench system header file
  **************************************************************************
  *
  * Copyright (c) 2025, Artery Technology, All rights reserved.
  * SPDX-License-Identifier: BSD-3-Clause
  *
  **************************************************************************
  */
/* define to prevent recursive inclusion -------------------------------------*/
#ifndef __WK_SYSTEM_H
#define __WK_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "at32m412_416.h"

void wk_timebase_init(void);
void wk_delay_us(uint32_t delay);
void wk_delay_ms(uint32_t delay);

#ifdef __cplusplus
}
#endif

#endif
