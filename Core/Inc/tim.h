/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

#include "gpio.h"

/* USER CODE END Includes */

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;

/* USER CODE BEGIN Private defines */

#define TIME_30_SEC	 30000
#define TIME_2_MIN	 120000
#define TIME_5_MIN	 300000
#define TIME_30_MIN	 1800000
#define PUMP_WORKING_TIME	2000 // odrediti koliko ce vremena pumpa raditi (u milisekundama)
extern uint32_t timer2_Ticks_Millisec;

/* USER CODE END Private defines */

void MX_TIM1_Init(void);
void MX_TIM2_Init(void);

/* USER CODE BEGIN Prototypes */

uint32_t timer2_get_millisec(void);
void timer2_wait_millisec(uint32_t ms);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

