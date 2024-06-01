/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "fatfs.h"
#include <string.h>
#include <stdio.h>
// Define LED pins
#define LED_YELLOW_Pin GPIO_PIN_0
#define LED_BLUE_Pin GPIO_PIN_1
#define LED_GREEN_Pin GPIO_PIN_2
#define LED_WHITE_Pin GPIO_PIN_3
#define LED_RED_Pin GPIO_PIN_5
#define LED_GPIO_Port GPIOC

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;

SPI_HandleTypeDef hspi1;

USART_HandleTypeDef husart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
static void MX_CAN2_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_Init(void);
/* USER CODE BEGIN PFP */
void CAN_FilterConfig(void);
void Log_CAN_Message(CAN_HandleTypeDef *hcan, char prefix);
void LED_Indication(char event);
void UART_Print(char *message);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
FATFS fs;
FIL file;
UINT bw;
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
    MX_CAN1_Init();
    MX_CAN2_Init();
    MX_SPI1_Init();
    MX_USART1_Init();
    MX_FATFS_Init();
    /* USER CODE BEGIN 2 */
    // Set CAN filters
    CAN_FilterConfig();
    // Mount the SD card
    LED_Indication('E');
    LED_Indication('S');
    if (f_mount(&fs, "", 1) != FR_OK)
    {
        // Mount failed, handle error

        LED_Indication('E'); // Indicate error with LED
    }

    // Open the log file
    if (f_open(&file, "log.txt", FA_OPEN_ALWAYS | FA_WRITE) != FR_OK)
    {
        // Open failed, handle error
        LED_Indication('E'); // Indicate error with LED
    }

    // Set CAN filters
    CAN_FilterConfig();

    // Start CAN
    if (HAL_CAN_Start(&hcan1) != HAL_OK)
    {
        // Start Error
        LED_Indication('E'); // Indicate error with LED
    }
    if (HAL_CAN_Start(&hcan2) != HAL_OK)
    {
        // Start Error
        LED_Indication('E'); // Indicate error with LED
    }

    // Activate CAN notifications
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
    HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);

    /* USER CODE END 2 */
    UART_Print("set up complete");
    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
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
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

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
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
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
 * @brief CAN1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_CAN1_Init(void)
{

    /* USER CODE BEGIN CAN1_Init 0 */

    /* USER CODE END CAN1_Init 0 */

    /* USER CODE BEGIN CAN1_Init 1 */

    /* USER CODE END CAN1_Init 1 */
    hcan1.Instance = CAN1;
    hcan1.Init.Prescaler = 8;
    hcan1.Init.Mode = CAN_MODE_NORMAL;
    hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan1.Init.TimeSeg1 = CAN_BS1_1TQ;
    hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;
    hcan1.Init.TimeTriggeredMode = DISABLE;
    hcan1.Init.AutoBusOff = DISABLE;
    hcan1.Init.AutoWakeUp = DISABLE;
    hcan1.Init.AutoRetransmission = DISABLE;
    hcan1.Init.ReceiveFifoLocked = DISABLE;
    hcan1.Init.TransmitFifoPriority = DISABLE;
    if (HAL_CAN_Init(&hcan1) != HAL_OK)
    {
        Error_Handler();
        UART_Print("errro setting up  can interface 1");
    }
    /* USER CODE BEGIN CAN1_Init 2 */

    /* USER CODE END CAN1_Init 2 */
}

/**
 * @brief CAN2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_CAN2_Init(void)
{

    /* USER CODE BEGIN CAN2_Init 0 */

    /* USER CODE END CAN2_Init 0 */

    /* USER CODE BEGIN CAN2_Init 1 */

    /* USER CODE END CAN2_Init 1 */
    hcan2.Instance = CAN2;
    hcan2.Init.Prescaler = 8;
    hcan2.Init.Mode = CAN_MODE_NORMAL;
    hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan2.Init.TimeSeg1 = CAN_BS1_1TQ;
    hcan2.Init.TimeSeg2 = CAN_BS2_1TQ;
    hcan2.Init.TimeTriggeredMode = DISABLE;
    hcan2.Init.AutoBusOff = DISABLE;
    hcan2.Init.AutoWakeUp = DISABLE;
    hcan2.Init.AutoRetransmission = DISABLE;
    hcan2.Init.ReceiveFifoLocked = DISABLE;
    hcan2.Init.TransmitFifoPriority = DISABLE;
    if (HAL_CAN_Init(&hcan2) != HAL_OK)
    {
        Error_Handler();
        UART_Print("errro setting up  can interface 2");
    }
    /* USER CODE BEGIN CAN2_Init 2 */

    /* USER CODE END CAN2_Init 2 */
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
    hspi1.Init.NSS = SPI_NSS_HARD_INPUT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi1) != HAL_OK)
    {
        Error_Handler();
        UART_Print("errror setting up  sd card interface");
    }
    /* USER CODE BEGIN SPI1_Init 2 */

    /* USER CODE END SPI1_Init 2 */
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_Init(void)
{

    /* USER CODE BEGIN USART1_Init 0 */

    /* USER CODE END USART1_Init 0 */

    /* USER CODE BEGIN USART1_Init 1 */

    /* USER CODE END USART1_Init 1 */
    husart1.Instance = USART1;
    husart1.Init.BaudRate = 115200;
    husart1.Init.WordLength = USART_WORDLENGTH_8B;
    husart1.Init.StopBits = USART_STOPBITS_1;
    husart1.Init.Parity = USART_PARITY_NONE;
    husart1.Init.Mode = USART_MODE_TX_RX;
    husart1.Init.CLKPolarity = USART_POLARITY_LOW;
    husart1.Init.CLKPhase = USART_PHASE_1EDGE;
    husart1.Init.CLKLastBit = USART_LASTBIT_DISABLE;
    if (HAL_USART_Init(&husart1) != HAL_OK)
    {
        Error_Handler();
        // Initialization Error
        LED_Indication('E'); // Indicate error with LED
        UART_Print("errro setting up  usart");
    }

    /* USER CODE BEGIN USART1_Init 2 */

    /* USER CODE END USART1_Init 2 */
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
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_RESET);

    /*Configure GPIO pins : PC0 PC1 PC2 PC3 */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* USER CODE BEGIN MX_GPIO_Init_2 */
    /* USER CODE END MX_GPIO_Init_2 */
    // Configure USART1 TX (PA9) and RX (PA10)
    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */
void UART_Print(char *message)
{
    HAL_USART_Transmit(&husart1, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
}

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

// CAN Filter Configuration
void CAN_FilterConfig(void)
{
    CAN_FilterTypeDef sFilterConfig;

    sFilterConfig.FilterBank = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.SlaveStartFilterBank = 14;

    if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
    {
        // Filter configuration Error
        LED_Indication('E'); // Indicate error with LED
    }

    sFilterConfig.FilterBank = 1;
    if (HAL_CAN_ConfigFilter(&hcan2, &sFilterConfig) != HAL_OK)
    {
        // Filter configuration Error
        LED_Indication('E'); // Indicate error with LED
    }
}

// Log CAN Message
void Log_CAN_Message(CAN_HandleTypeDef *hcan, char prefix)
{
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8];
    char log_entry[50];

    // Get the message
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
    {
        // Reception Error
        LED_Indication('E'); // Indicate error with LED
        if (prefix == 'A')
        {
            UART_Print("error getting messages from   can interface 1");
            LED_Indication('1');
        }
        else
        {
            UART_Print("error getting messages from   can interface 2");
            LED_Indication('2');
        }
        return;
    }

    // Format the log entry
    snprintf(log_entry, sizeof(log_entry), "%c ID: 0x%03X DLC: %d Data: ", prefix, (unsigned int)RxHeader.StdId, (unsigned int)RxHeader.DLC);
    for (int i = 0; i < RxHeader.DLC; i++)
    {
        snprintf(&log_entry[strlen(log_entry)], sizeof(log_entry) - strlen(log_entry), "%02X ", RxData[i]);
    }
    snprintf(&log_entry[strlen(log_entry)], sizeof(log_entry) - strlen(log_entry), "\r\n");

    // Write to the log file
    if (f_write(&file, log_entry, strlen(log_entry), &bw) != FR_OK)
    {
        // Write Error
        LED_Indication('E'); // Indicate error with LED
        return;
    }

    // Ensure data is written to the disk
    f_sync(&file);
    LED_Indication('S'); // Indicate success with LED
    // Print to UART
    UART_Print("success writing to disk");
    UART_Print(log_entry);
}
/*
 * Callback Function for Receiving CAN Messages
This function is called automatically when a CAN message is received in RX FIFO0:
 * */
// CAN RX FIFO0 message pending callback
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if (hcan->Instance == CAN1)
    {
        Log_CAN_Message(hcan, 'A');
    }
    else if (hcan->Instance == CAN2)
    {
        Log_CAN_Message(hcan, 'B');
    }
}

// LED Indication Function
void LED_Indication(char event)
{
    switch (event)
    {
    case 'S': // Success: Blink LED on PA15
        for (int i = 0; i < 3; i++)
        {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_2);
            HAL_Delay(100);
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_2);
            HAL_Delay(100);
        }
        break;
    case 'E': // Error: Blink LED on PC10, PC11, PC12
        for (int i = 0; i < 3; i++)
        {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3);
            HAL_Delay(500);
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3);
            HAL_Delay(500);
        }
        break;
    case '1': // Error: Blink LED on PC10, PC11, PC12
        for (int i = 0; i < 5; i++)
        {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_3);
            HAL_Delay(2000);
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_3);
            HAL_Delay(500);
        }
        break;
    case '2': // Error: Blink LED on PC10, PC11, PC12
        for (int i = 0; i < 5; i++)
        {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_1);
            HAL_Delay(1000);
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_1);
            HAL_Delay(500);
        }
        break;
    default:
        break;
    }
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
