/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include <string.h>

char RX_BUFFER[BUFSIZE];
int RX_BUFFER_HEAD;
/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PB6     ------> USART1_TX
    PB7     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PB6     ------> USART1_TX
    PB7     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void USER_UART_IRQHandler(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART1) {
		uint8_t rx_data = __HAL_UART_FLUSH_DRREGISTER(huart);

		if (RX_BUFFER_HEAD + 1 < BUFSIZE) {
			RX_BUFFER[RX_BUFFER_HEAD++] = rx_data;
		}
	}
}

void USART1_SendChar(uint8_t c) {
	HAL_UART_Transmit(&huart1, &c, sizeof(c), 10);
}

void USART1_SendString(char *s, uint16_t len) {
	if(len > 115) {
		// ovo je potrebno jer ESP iz nekog razloga ne moze primiti vise od 120 bajta odjednom
		for(int i = 0; i < len; i++) {
			USART1_SendChar(s[i]);
		}
	} else {
		HAL_UART_Transmit(&huart1, (uint8_t *) s, len, 10);
	}
}

void USART1_ClearBuffer() {
	HAL_NVIC_DisableIRQ(USART1_IRQn);

	for(int i = 0; i < RX_BUFFER_HEAD; i++) {
		RX_BUFFER[i] = 0;
	}

	RX_BUFFER_HEAD = 0;

	HAL_NVIC_EnableIRQ(USART1_IRQn);
}

uint16_t USART1_GetBufferSize() {
	return BUFSIZE;
}

void USART1_GetBufferContent(char *dest) {
	HAL_NVIC_DisableIRQ(USART1_IRQn);
	strcpy(dest, RX_BUFFER);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
}

int8_t USART1_WaitFor(char *successMsg, char *errorMsg, uint16_t timeout) {
	uint32_t start = HAL_GetTick();
	uint32_t diff = 0;

	char bufcopy[BUFSIZE];
	USART1_GetBufferContent(bufcopy);

	while(!(strstr(bufcopy, successMsg) || strstr(bufcopy, errorMsg) || (diff > timeout))) {
		diff = HAL_GetTick() - start;
		USART1_GetBufferContent(bufcopy);
	}

	if(strstr(bufcopy, errorMsg) || diff > timeout) {
		return -1;
	}

	return 0;

}

int8_t USART1_WaitForTrueOrFalse(char *errorMsg, uint16_t timeout) {
	uint32_t start = HAL_GetTick();
	uint32_t diff = 0;

	char bufcopy[BUFSIZE];
	USART1_GetBufferContent(bufcopy);

	while(!(strstr(bufcopy, "True") || strstr(bufcopy, "False") || strstr(bufcopy, errorMsg) || (diff > timeout))) {
		diff = HAL_GetTick() - start;
		USART1_GetBufferContent(bufcopy);
	}

	if(strstr(bufcopy, "True")) {
		return 1;
	} else if(strstr(bufcopy, "False")) {
		return 0;
	}

	return -1;

}
/* USER CODE END 1 */
