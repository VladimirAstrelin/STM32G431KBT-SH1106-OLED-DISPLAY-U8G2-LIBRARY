/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

// Библиотека U8g2 для работы с OLED дисплеями SH1106/SSD1306
#include "u8g2.h"
#include <stdbool.h>    // Для типа bool (true/false)
#include <string.h>     // Для строковых функций (strcmp, strlen, memset)
#include <stdio.h>      // Для sprintf

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define BUTTON_PRESSED   GPIO_PIN_RESET   // 0 - кнопка нажата
#define BUTTON_RELEASED  GPIO_PIN_SET     // 1 - кнопка отпущена

#define DEBOUNCE_DELAY   50

// Настройки пинов для подключения OLED дисплея по SPI
/* =========================================================
   DISPLAY PINS
========================================================= */

#define OLED_CS_PORT   GPIOB      // CS (Chip Select) - порт
#define OLED_CS_PIN    GPIO_PIN_0 // CS (Chip Select) - пин PB0

#define OLED_DC_PORT   GPIOA      // DC (Data/Command) - порт
#define OLED_DC_PIN    GPIO_PIN_4 // DC (Data/Command) - пин PA4

#define OLED_RST_PORT  GPIOA      // RST (Reset) - порт
#define OLED_RST_PIN   GPIO_PIN_6 // RST (Reset) - пин PA6

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */

// Объект дисплея для библиотеки U8g2
// Хранит всё состояние дисплея (буфер, шрифты, настройки интерфейса)
u8g2_t u8g2;

// Счётчик для анимации
uint16_t counter = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// Прототипы функций для u8g2
void u8g2_delay_us(uint32_t us);
uint8_t u8g2_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8g2_byte_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

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
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  HAL_GPIO_WritePin(GPIOB, LED_PB8_Pin, GPIO_PIN_SET);

  // ============================================================
  // ИНИЦИАЛИЗАЦИЯ OLED ДИСПЛЕЯ SH1106 (SPI)
  // ============================================================

  // Начальная установка пинов CS (выкл), DC (команда), RES (сброс)
  HAL_GPIO_WritePin(OLED_CS_PORT, OLED_CS_PIN, GPIO_PIN_SET);     // CS = HIGH (дисплей не выбран)
  HAL_GPIO_WritePin(OLED_DC_PORT, OLED_DC_PIN, GPIO_PIN_RESET);   // DC = 0 (команда)
  HAL_GPIO_WritePin(OLED_RST_PORT, OLED_RST_PIN, GPIO_PIN_RESET); // RESET = LOW

  HAL_Delay(10);  // Держим RESET минимум 10 мс

  HAL_GPIO_WritePin(OLED_RST_PORT, OLED_RST_PIN, GPIO_PIN_SET); // RESET = HIGH (выход из сброса)
  HAL_Delay(100); // Ожидание после сброса

  // Инициализация дисплея SH1106 с интерфейсом SPI
  // u8g2_byte_hw_spi  - наша функция отправки байта через HAL SPI
  // u8g2_gpio_and_delay - наша функция управления GPIO и задержками
  // суффикс _f — это полный буфер (1024 байта)
  u8g2_Setup_sh1106_128x64_noname_f(&u8g2,
                                     U8G2_R0,             // Ориентация (0 градусов)
                                     u8g2_byte_hw_spi,    // Функция отправки байта (наш HAL SPI)
                                     u8g2_gpio_and_delay  // Функции GPIO и задержки
                                    );

  u8g2_InitDisplay(&u8g2);     // Инициализация дисплея (отправка команд)
  u8g2_SetPowerSave(&u8g2, 0); // Включение дисплея (0 = питание вкл)

  // Первая отрисовка — заставка
  u8g2_ClearBuffer(&u8g2);
  u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
  u8g2_DrawStr(&u8g2, 10, 20, "OLED SH1106 OK");
  u8g2_DrawStr(&u8g2, 10, 40, "SPI Mode");
  u8g2_DrawStr(&u8g2, 10, 60, "STM32G431");
  u8g2_SendBuffer(&u8g2);

  HAL_Delay(2000); // Показываем заставку 2 секунды

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	    // Очищаем буфер дисплея (внутренний буфер u8g2)
	    u8g2_ClearBuffer(&u8g2);

	    // Устанавливаем шрифт
	    u8g2_SetFont(&u8g2, u8g2_font_helvB08_tr);

	    // Рисуем рамку
	    u8g2_DrawFrame(&u8g2, 0, 0, 128, 64);

	    // Выводим текст
	    u8g2_DrawStr(&u8g2, 5, 15, "TEST SH1106");

	    // Выводим счётчик
	    char counter_str[16];
	    sprintf(counter_str, "Counter: %04d", counter);
	    u8g2_DrawStr(&u8g2, 5, 35, counter_str);

	    // Рисуем простой индикатор прогресса
	    uint8_t progress = (counter % 100) * 118 / 100; // 0-118 пикселей
	    u8g2_DrawBox(&u8g2, 5, 48, progress, 8);

	    // Отправляем буфер на дисплей
	    u8g2_SendBuffer(&u8g2);

	    // Мигаем бортовым LED каждые 500 мс (один цикл отрисовки ~300 мс)
	    if ((counter % 50) < 25) {
	        HAL_GPIO_WritePin(GPIOB, LED_PB8_Pin, GPIO_PIN_RESET); // LED ON
	    } else {
	        HAL_GPIO_WritePin(GPIOB, LED_PB8_Pin, GPIO_PIN_SET);   // LED OFF
	    }

	    counter++;
	    HAL_Delay(250); // Пауза 250 мс между кадрами

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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV6;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|LED_PB8_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA4 PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 LED_PB8_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|LED_PB8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

// Функция задержки в микросекундах (для u8g2)
void u8g2_delay_us(uint32_t us) {
    // HAL_Delay работает только с миллисекундами
    // Для микросекунд используем простой цикл
    // При частоте ядра ~170 МГц этот цикл даёт приблизительную задержку
    // Для инициализации дисплея точность достаточна
    while (us--) {
        for (volatile int i = 0; i < 10; i++);
    }
}

// Функция отправки байта через аппаратный SPI HAL (для u8g2)
// Управляет CS и DC вокруг передачи данных/команд
uint8_t u8g2_byte_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
        case U8X8_MSG_BYTE_SEND:
            // Отправляем байты через HAL SPI
            HAL_SPI_Transmit(&hspi1, (uint8_t *)arg_ptr, arg_int, HAL_MAX_DELAY);
            break;

        case U8X8_MSG_BYTE_INIT:
            // Инициализация — ничего не нужно, HAL SPI уже инициализирован
            break;

        case U8X8_MSG_BYTE_SET_DC:
            // Устанавливаем DC (Data/Command)
            if (arg_int == 0)
                HAL_GPIO_WritePin(OLED_DC_PORT, OLED_DC_PIN, GPIO_PIN_RESET); // Command
            else
                HAL_GPIO_WritePin(OLED_DC_PORT, OLED_DC_PIN, GPIO_PIN_SET);   // Data
            break;

        case U8X8_MSG_BYTE_START_TRANSFER:
            // Опускаем CS — начало передачи
            HAL_GPIO_WritePin(OLED_CS_PORT, OLED_CS_PIN, GPIO_PIN_RESET);
            break;

        case U8X8_MSG_BYTE_END_TRANSFER:
            // Поднимаем CS — конец передачи
            HAL_GPIO_WritePin(OLED_CS_PORT, OLED_CS_PIN, GPIO_PIN_SET);
            break;

        default:
            return 0;
    }
    return 1;
}

// Функция управления GPIO и задержками для u8g2
uint8_t u8g2_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
        case U8X8_MSG_GPIO_AND_DELAY_INIT:   // Инициализация GPIO и задержек
            break;

        case U8X8_MSG_DELAY_MILLI:           // Задержка в миллисекундах
            HAL_Delay(arg_int);
            break;

        case U8X8_MSG_DELAY_10MICRO:         // Задержка в 10 микросекундах
            u8g2_delay_us(arg_int * 10);
            break;

        case U8X8_MSG_DELAY_100NANO:         // Задержка в 100 наносекундах (приблизительно)
            u8g2_delay_us(arg_int / 10);
            break;

        case U8X8_MSG_GPIO_RESET:            // Управление RESET
            if (arg_int == 0)
                HAL_GPIO_WritePin(OLED_RST_PORT, OLED_RST_PIN, GPIO_PIN_RESET);
            else
                HAL_GPIO_WritePin(OLED_RST_PORT, OLED_RST_PIN, GPIO_PIN_SET);
            break;
    }
    return 1;
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
#ifdef USE_FULL_ASSERT
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
