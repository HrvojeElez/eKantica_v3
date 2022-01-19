/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>

#include "hcsr04.h"
#include "wifi.h"
#include "bme280.h"
#include "sh.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
SH senzor_tlo;
uint8_t errNum = 0;
BME280 senzor_zrak;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_I2C3_Init();
  MX_ADC3_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
  HAL_TIM_Base_Start_IT(&htim2);

  uint8_t errors =  BME280_Initialise(&senzor_zrak, &hi2c3);

    if(errors != 0)
    {
  	  HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,GPIO_PIN_SET);
  	  while(1);
    }

    int8_t err = WIFI_Init("dlink-F7E6", "bdqjy77979");

    if(err != 0)
    {
  	  HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,GPIO_PIN_SET);
  	  while(1);
    }

    errors = BME280_PerformMeasurements(&senzor_zrak);

    if(errors != 0)
    {
  	  HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,GPIO_PIN_SET);
  	  while(1);
    }

    SH_init(&senzor_tlo, &hadc3);

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    // wait for 2 minutes, perform measurements, send them to server, water the plant
    while (1)
    {

  	  timer2_wait_millisec(TIME_2_MIN);

  	  HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,GPIO_PIN_SET);
  	  HAL_Delay(500);
  	  HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,GPIO_PIN_RESET);
  	  err = BME280_ReadData(&senzor_zrak);

  	 if(err != HAL_OK)
  	  {
  		  HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,GPIO_PIN_SET);
  		  while(1);
  	  }

  	  errors = SH_ReadData(&senzor_tlo);
  	  if(errors != 0)
  	  {
  		  HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,GPIO_PIN_SET);
  		  while(1);
  	  }

  	  HCSR04_Read();

      // ne brisati timer jer ce hsr04 mozda poslati staru vrijednost
  	  HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,GPIO_PIN_SET);
  	  HAL_Delay(5000);
  	  HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,GPIO_PIN_RESET);
	  HAL_Delay(500);

  	  int8_t val = WIFI_SendRequestWithParams("ekantica.herokuapp.com", "/data",(double) senzor_zrak.temp, (double) senzor_zrak.hum, (double)senzor_tlo.soilHumidity, (double) Distance);

  	  if(val == 0)
  	  {
  	  	  // ne treba zaliti; LUKA nadopisi


  	  }
  	  else if(val == 1)
  	  {
  		  // zalij LUKA nadopisi

  	  }
  	  else
  	  {
  		//error; kratak blink nakon errora
  	  	  HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,GPIO_PIN_SET);
  	  	  HAL_Delay(500);
  	  	  HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,GPIO_PIN_RESET);
  	  	  HAL_Delay(1000);
  	  }
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void Mjerenje_Vrijednosti(){

}

void Zalij_Ako_Je_Suho(){

}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

