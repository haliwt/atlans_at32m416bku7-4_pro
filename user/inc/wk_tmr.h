/* add user code begin Header */
/**
  **************************************************************************
  * @file     wk_tmr.h
  * @brief    header file of work bench config
  **************************************************************************
  *
  * Copyright (c) 2025, Artery Technology, All rights reserved.
  * SPDX-License-Identifier: BSD-3-Clause
  *
  **************************************************************************
  */
/* add user code end Header */

/* define to prevent recursive inclusion -----------------------------------*/
#ifndef __WK_TMR_H
#define __WK_TMR_H

#ifdef __cplusplus
extern "C" {
#endif

/* includes -----------------------------------------------------------------------*/
#include "at32m412_416_wk_config.h"

/* private includes -------------------------------------------------------------*/
/* add user code begin private includes */

/* add user code end private includes */

/* exported types -------------------------------------------------------------*/
/* add user code begin exported types */

/* add user code end exported types */

/* exported constants --------------------------------------------------------*/
/* add user code begin exported constants */

/* add user code end exported constants */

/* exported macro ------------------------------------------------------------*/
/* add user code begin exported macro */

/* add user code end exported macro */

/* exported functions ------------------------------------------------------- */

  /* init tmr3 function. */
  void wk_tmr3_init(void);

  /* init tmr4 function. */
  void wk_tmr4_init(void);

  /* init tmr10 function. */
  void wk_tmr10_init(void);

  /* init tmr11 function. */
  void wk_tmr11_init(void);

/* add user code begin exported functions */

/* add user code end exported functions */

#ifdef __cplusplus
}
#endif

#endif
