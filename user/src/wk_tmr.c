/* add user code begin Header */
/**
  **************************************************************************
  * @file     wk_tmr.c
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
#include "wk_tmr.h"

/* add user code begin 0 */

/* add user code end 0 */

/**
  * @brief  init tmr3 function.
  * @param  none  FAN PWM IS 25KHZ
  * @retval none
  */
void wk_tmr3_init(void)
{
  /* add user code begin tmr3_init 0 */

  /* add user code end tmr3_init 0 */

  gpio_init_type gpio_init_struct;
  tmr_output_config_type tmr_output_struct;

  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin tmr3_init 1 */
   crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE); //WT.EDIT 2026-11-18
  /* add user code end tmr3_init 1 */

  /* configure the tmr3 CH2 pin */
  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE4, GPIO_MUX_2);
  gpio_init_struct.gpio_pins = GPIO_PINS_4;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init(GPIOA, &gpio_init_struct);

  /* configure counter settings */
  tmr_cnt_dir_set(TMR3, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR3, TMR_CLOCK_DIV1);
  tmr_period_buffer_enable(TMR3, FALSE);
  tmr_base_init(TMR3, 7199, 0);

  /* configure primary mode settings */
  tmr_sub_sync_mode_set(TMR3, FALSE);
  tmr_primary_mode_select(TMR3, TMR_PRIMARY_SEL_RESET);

  /* configure channel 2 output settings */
  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;//TMR_OUTPUT_CONTROL_OFF;
  tmr_output_struct.oc_output_state = TRUE;
  tmr_output_struct.occ_output_state = FALSE;
  tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.oc_idle_state = FALSE;
  tmr_output_struct.occ_idle_state = FALSE;
  tmr_output_channel_config(TMR3, TMR_SELECT_CHANNEL_2, &tmr_output_struct);
  tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, 0);
  tmr_output_channel_buffer_enable(TMR3, TMR_SELECT_CHANNEL_2, FALSE);

  tmr_counter_enable(TMR3, TRUE);

  /* add user code begin tmr3_init 2 */

  /* add user code end tmr3_init 2 */
}

/**
  * @brief  init tmr4 function. DIM_PWM
  * @param  none DIM_PWM 1KHZ
  * @retval none
  */
void wk_tmr4_init(void)
{
  /* add user code begin tmr4_init 0 */

  /* add user code end tmr4_init 0 */

  gpio_init_type gpio_init_struct;
  tmr_output_config_type tmr_output_struct;

  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin tmr4_init 1 */
   crm_periph_clock_enable(CRM_TMR4_PERIPH_CLOCK, TRUE); //WT.EDIT 2026-11-18
  /* add user code end tmr4_init 1 */

  /* configure the tmr4 CH3 pin */
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE8, GPIO_MUX_2);
  gpio_init_struct.gpio_pins = GPIO_PINS_8;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init(GPIOB, &gpio_init_struct);

  /* configure counter settings */
  tmr_cnt_dir_set(TMR4, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR4, TMR_CLOCK_DIV1);
  tmr_period_buffer_enable(TMR4, FALSE);
  tmr_base_init(TMR4, 59999, 2);

  /* configure primary mode settings */
  tmr_sub_sync_mode_set(TMR4, FALSE);
  tmr_primary_mode_select(TMR4, TMR_PRIMARY_SEL_RESET);

  /* configure channel 3 output settings */
  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;//TMR_OUTPUT_CONTROL_OFF;
  tmr_output_struct.oc_output_state = TRUE;
  tmr_output_struct.occ_output_state = FALSE;
  tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.oc_idle_state = FALSE;
  tmr_output_struct.occ_idle_state = FALSE;
  tmr_output_channel_config(TMR4, TMR_SELECT_CHANNEL_3, &tmr_output_struct);
  tmr_channel_value_set(TMR4, TMR_SELECT_CHANNEL_3, 0);
  tmr_output_channel_buffer_enable(TMR4, TMR_SELECT_CHANNEL_3, FALSE);

  tmr_counter_enable(TMR4, TRUE);

  /* add user code begin tmr4_init 2 */

  /* add user code end tmr4_init 2 */
}

/**
  * @brief  init tmr10 function.
  * @param  none BUZZER IS 4KHZ
  * @retval none
  */
void wk_tmr10_init(void)
{
  /* add user code begin tmr10_init 0 */

  /* add user code end tmr10_init 0 */

  gpio_init_type gpio_init_struct;
  tmr_output_config_type tmr_output_struct;
  tmr_brkdt_config_type tmr_brkdt_struct;

  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin tmr10_init 1 */
   crm_periph_clock_enable(CRM_TMR10_PERIPH_CLOCK, TRUE); //WT.EDIT 2026-11-18

  /* add user code end tmr10_init 1 */

  /* configure the tmr10 CH1 pin */
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE4, GPIO_MUX_11);
  gpio_init_struct.gpio_pins = GPIO_PINS_4;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init(GPIOB, &gpio_init_struct);

  /* configure counter settings */
  tmr_cnt_dir_set(TMR10, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR10, TMR_CLOCK_DIV1);
  tmr_repetition_counter_set(TMR10, 0);
  tmr_period_buffer_enable(TMR10, FALSE);
  tmr_base_init(TMR10, 249, 179);

  /* configure channel 1 output settings */
  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_output_struct.oc_output_state = TRUE;
  tmr_output_struct.occ_output_state = FALSE;
  tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.oc_idle_state = FALSE;
  tmr_output_struct.occ_idle_state = FALSE;
  tmr_output_channel_config(TMR10, TMR_SELECT_CHANNEL_1, &tmr_output_struct);
  tmr_channel_value_set(TMR10, TMR_SELECT_CHANNEL_1, 0);
  tmr_output_channel_buffer_enable(TMR10, TMR_SELECT_CHANNEL_1, FALSE);

  tmr_output_channel_immediately_set(TMR10, TMR_SELECT_CHANNEL_1, FALSE);

  /* configure break and dead-time settings */
  tmr_brkdt_struct.brk_enable = FALSE;
  tmr_brkdt_struct.auto_output_enable = FALSE;
  tmr_brkdt_struct.brk_polarity = TMR_BRK_INPUT_ACTIVE_LOW;
  tmr_brkdt_struct.fcsoen_state = FALSE;
  tmr_brkdt_struct.fcsodis_state = FALSE;
  tmr_brkdt_struct.wp_level = TMR_WP_OFF;
  tmr_brkdt_struct.deadtime = 0;
  tmr_brkdt_config(TMR10, &tmr_brkdt_struct);
  tmr_brk_filter_value_set(TMR10, 0);

  tmr_output_enable(TMR10, TRUE);

  tmr_counter_enable(TMR10, TRUE);

  /* add user code begin tmr10_init 2 */

  /* add user code end tmr10_init 2 */
}

/**
  * @brief  init tmr11 function.
  * @param  none
  * @retval none
  */
void wk_tmr11_init(void)
{
  
	/* add user code begin tmr11_init 0 */
	
	 /* add user code end tmr11_init 0 */
	 // T = 10ms
	
	 /* add user code begin tmr11_init 1 */
	
	 /* add user code end tmr11_init 1 */
	 /* 1. 必须先开启 TMR11 外设总线时钟 */
       crm_periph_clock_enable(CRM_TMR11_PERIPH_CLOCK, TRUE);
	 /* configure counter settings */
	 tmr_cnt_dir_set(TMR11, TMR_COUNT_UP);
	 tmr_clock_source_div_set(TMR11, TMR_CLOCK_DIV1);
	 tmr_repetition_counter_set(TMR11, 0);
	 tmr_period_buffer_enable(TMR11, FALSE);
	 tmr_base_init(TMR11, 9999, 179);

	 /* 2. 必须使能 TMR11 溢出中断源（核心遗漏点） */
      tmr_interrupt_enable(TMR11, TMR_OVF_INT, TRUE);
	 /* 3. 必须配置并使能 CPU 的 NVIC 中断通道 */
    // nvic_irq_enable(TMR1_TRG_HALL_TMR11_IRQn, 1, 0); // 优先级可根据项目自行调整
     
	 tmr_counter_enable(TMR11, TRUE);
}

/* add user code begin 1 */

/* add user code end 1 */
