/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define ASYNC_MODE
//#define DE_RE_MODE
#define MODBUS_REQ_LEN 8
#define MODBUS_RES_LEN 9
#define MODBUS_TIMEOUT 1000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
static uint8_t modbus_req[] = { 0x01, 0x04, 0x00, 0x01, 0x00, 0x02, 0x20, 0x0B };
static uint8_t modbus_res[MODBUS_RES_LEN] = { 0 };

static uint8_t msg_buf[256] = { 0 };

static char receive_str[] = "Receive\r\n";

volatile uint8_t tx_complete = 0;
volatile uint8_t rx_complete = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static uint8_t getHexValue(uint8_t a) {
  if (a < 10) {
    return '0' + a;
  }

  return 'A' + (a - 10);
}

static void printByteArray(uint8_t *arr, int len) {
  int index = -1;
  for (int i = 0; i < len; ++i) {
    uint8_t hi = (arr[i] >> 4) & 0xF;
    uint8_t lo = arr[i] & 0xF;
    msg_buf[++index] = getHexValue(hi);
    msg_buf[++index] = getHexValue(lo);
    msg_buf[++index] = ' ';
  }

  msg_buf[++index] = '\n';
  msg_buf[++index] = '\r';
  msg_buf[++index] = 0;
}

static void printTempArray(uint8_t *arr, int len) {
  int index = -1;
  int v = (arr[3] << 8) + arr[4];

  uint8_t a = v / 100;
  uint8_t b = (v / 10) % 10;
  uint8_t c = v % 10;

  msg_buf[++index] = 'T';
  msg_buf[++index] = '=';
  msg_buf[++index] = a + '0';
  msg_buf[++index] = b + '0';
  msg_buf[++index] = '.';
  msg_buf[++index] = c + '0';
  msg_buf[++index] = '\n';
  msg_buf[++index] = '\r';
  msg_buf[++index] = 0;
}

static void printHumArray(uint8_t *arr, int len) {
  int index = -1;
  int v = (arr[5] << 8) + arr[6];

  uint8_t a = v / 100;
  uint8_t b = (v / 10) % 10;
  uint8_t c = v % 10;

  msg_buf[++index] = 'H';
  msg_buf[++index] = '=';
  msg_buf[++index] = a + '0';
  msg_buf[++index] = b + '0';
  msg_buf[++index] = '.';
  msg_buf[++index] = c + '0';
  msg_buf[++index] = '\n';
  msg_buf[++index] = '\r';
  msg_buf[++index] = 0;
}


#ifdef ASYNC_MODE
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART3) {
    tx_complete = 1;

    #ifdef DE_RE_MODE
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
    #endif
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART3) {
    rx_complete = 1;
  }
}
#endif

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
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  char started_str[] = "Started\r\n";
  char send_str[] = "Send\r\n";

  HAL_UART_Transmit(&huart2, (uint8_t*)started_str, strlen(started_str), HAL_MAX_DELAY);

  #ifdef ASYNC_MODE
  HAL_UART_Receive_IT(&huart3, (uint8_t*)modbus_res, MODBUS_RES_LEN);
  #ifdef DE_RE_MODE
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET); // Enable Transmit Mode
  #endif
  HAL_UART_Transmit_IT(&huart3, (uint8_t*)modbus_req, MODBUS_REQ_LEN);
  #endif

  while (1)
  {
    #ifndef ASYNC_MODE
    #ifdef DE_RE_MODE
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET); // Enable Transmit Mode
    #endif
    HAL_UART_Transmit(&huart3, (uint8_t*)modbus_req, MODBUS_REQ_LEN, MODBUS_TIMEOUT);
    #ifdef DE_RE_MODE
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
    #endif
    HAL_UART_Receive(&huart3, (uint8_t*)modbus_res, MODBUS_RES_LEN, MODBUS_TIMEOUT);
    #endif

    #ifdef ASYNC_MODE
    if (rx_complete) {
      rx_complete = 0;
      HAL_UART_Transmit(&huart2, (uint8_t*)send_str, strlen(send_str), HAL_MAX_DELAY);
      printByteArray(modbus_req, MODBUS_REQ_LEN);
      HAL_UART_Transmit(&huart2, (uint8_t*)msg_buf, strlen(msg_buf), HAL_MAX_DELAY);

      HAL_UART_Transmit(&huart2, (uint8_t*)receive_str, strlen(receive_str), HAL_MAX_DELAY);
      printByteArray(modbus_res, MODBUS_RES_LEN);
      HAL_UART_Transmit(&huart2, (uint8_t*)msg_buf, strlen(msg_buf), HAL_MAX_DELAY);
  
      printTempArray(modbus_res, MODBUS_RES_LEN);
      HAL_UART_Transmit(&huart2, (uint8_t*)msg_buf, strlen(msg_buf), HAL_MAX_DELAY);
      printHumArray(modbus_res, MODBUS_RES_LEN);
      HAL_UART_Transmit(&huart2, (uint8_t*)msg_buf, strlen(msg_buf), HAL_MAX_DELAY);


      HAL_UART_Receive_IT(&huart3, (uint8_t*)modbus_res, MODBUS_RES_LEN);
      #ifdef DE_RE_MODE
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET); // Enable Transmit Mode
      #endif
      HAL_UART_Transmit_IT(&huart3, (uint8_t*)modbus_req, MODBUS_REQ_LEN);
    }
    #else
    HAL_UART_Transmit(&huart2, (uint8_t*)send_str, strlen(send_str), HAL_MAX_DELAY);
    printByteArray(modbus_req, MODBUS_REQ_LEN);
    HAL_UART_Transmit(&huart2, (uint8_t*)msg_buf, strlen(msg_buf), HAL_MAX_DELAY);

    HAL_UART_Transmit(&huart2, (uint8_t*)receive_str, strlen(receive_str), HAL_MAX_DELAY);
    printByteArray(modbus_res, MODBUS_RES_LEN);
    HAL_UART_Transmit(&huart2, (uint8_t*)msg_buf, strlen(msg_buf), HAL_MAX_DELAY);

    printTempArray(modbus_res, MODBUS_RES_LEN);
    HAL_UART_Transmit(&huart2, (uint8_t*)msg_buf, strlen(msg_buf), HAL_MAX_DELAY);
    printHumArray(modbus_res, MODBUS_RES_LEN);
    HAL_UART_Transmit(&huart2, (uint8_t*)msg_buf, strlen(msg_buf), HAL_MAX_DELAY);
    #endif

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    HAL_Delay(1000);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_Delay(1000);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
