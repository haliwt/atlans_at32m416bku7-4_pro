/* add user code begin Header */
/**
  **************************************************************************
  * @file     at32m412_416_int.h
  * @brief    header file of main interrupt service routines.
  **************************************************************************
  *
  * Copyright (c) 2025, Artery Technology, All rights reserved.
  * SPDX-License-Identifier: BSD-3-Clause
  *
  **************************************************************************
  */
/* add user code end Header */

/* define to prevent recursive inclusion -------------------------------------*/
#ifndef __AT32M412_416_INT_H
#define __AT32M412_416_INT_H

#ifdef __cplusplus
extern "C" {
#endif

/* includes ------------------------------------------------------------------*/
#include "at32m412_416.h"

/* private includes ----------------------------------------------------------*/
/* add user code begin private includes */

/* add user code end private includes */

/* exported types ------------------------------------------------------------*/
/* add user code begin exported types */

/* add user code end exported types */

/* exported constants --------------------------------------------------------*/
/* add user code begin exported constants */

/* add user code end exported constants */

/* exported macro ------------------------------------------------------------*/
/* add user code begin exported macro */

/* add user code end exported macro */

/* exported functions ------------------------------------------------------- */
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void TMR1_TRG_HALL_TMR11_IRQHandler(void);
/* add user code begin exported functions */

/* add user code end exported functions */

#ifdef __cplusplus
}
#endif

#endif
