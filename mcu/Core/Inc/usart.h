/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */

#define USART1_DMA_REC_SIZE 600
#define USART1_REC_SIZE 1200
#define USART2_DMA_REC_SIZE 600
#define USART2_REC_SIZE 1200

typedef struct
{
    uint8_t UsartRecFlag;                               // 标志位
    uint16_t UsartRecLen;                               // 接收数据长度
    uint16_t UsartDMARecLEN;                            // DMA 接收长度
    uint8_t  UsartDMARecBuffer[USART1_DMA_REC_SIZE];    // DMA 接收数组
    uint8_t  UsartRecBuffer[USART1_REC_SIZE];           // 接收组
}UsartType;

extern UsartType Usart1type, Usart2type;

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */

void my_uart_init(void);
void my_uart1_send(uint8_t *tdata,uint16_t tnum);
void my_uart1_send_variable(uint8_t *tdata);
void my_uart2_send(uint8_t *tdata,uint16_t tnum);
void my_uart2_send_variable(uint8_t *tdata);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

