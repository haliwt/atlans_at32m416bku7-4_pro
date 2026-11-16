/* add user code begin Header */
/**
  **************************************************************************
  * @file     wk_wdt.c
  * @brief    work bench config program
  **************************************************************************
  *
  * Copyright (c) 2025, Artery Technology, All rights reserved.
  * SPDX-License-Identifier: BSD-3-Clause
  *
  **************************************************************************
  */
/* add user code end Header */

/* Includes ------------------------------------------------------------------*/
#include "wk_wdt.h"

/* add user code begin 0 */

/* add user code end 0 */

/**
  * @brief  init wdt function.
  * @param  none
  * @retval none
  */
void wk_wdt_init(void)
{
  /* add user code begin wdt_init 0 */

  /* add user code end wdt_init 0 */

  wdt_register_write_enable(TRUE);
  wdt_divider_set(WDT_CLK_DIV_4);
  wdt_reload_value_set(0xFFF);
  wdt_counter_reload();

  /* if enabled, please feed the dog through wdt_counter_reload() function */
  //wdt_enable();

  /* add user code begin wdt_init 1 */

  /* add user code end wdt_init 1 */
}

/* add user code begin 1 */

/* add user code end 1 */
