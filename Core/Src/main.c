/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "dma.h"
#include "spi.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "W25Qxx.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t wData[0x1000];   //?????
uint8_t rData[0x1000];   //?????
uint8_t ID[4];          //??ID????
uint32_t i;
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
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
	printf("\r\n SPI-W25Qxx Example \r\n\r\n");
	/*-Step1- ????ID  ************************************************Step1*/ 
//	BSP_W25Qx_Init();
//	BSP_W25Qx_Read_ID(ID);
//    //?????ID??0xEF,?????ID??????,???:
//     //W25Q16?0x14?32?0x15?40?0x12?64?0x16?80?0x13?128?0x17
//	if((ID[0] != 0xEF) | (ID[1] != 0x17)) 
//	{                                
//		printf("something wrong in Step1 \r\n");
//		HAL_UART_Transmit(&huart1, (const uint8_t *)ID, 2, 1000);
//	}
//	else
//	{
//		printf(" W25Qxx ID is : 0x%02X%02X \r\n", ID[0], ID[1]);
//	}
//	/*-Step2- ???  ************************************************Step2*/ 	
//	if(BSP_W25Qx_Erase_Block(0) == W25Qx_OK)
//		printf(" QSPI Erase Block OK!\r\n");
//	else
//			printf("something wrong in Step2\r\n");
//	/*-Step3- ???  ************************************************Step3*/	
//	for(i =0;i<0x1000;i ++)
//	{
//			wData[i] = i % 256;
//      rData[i] = 0;
//	}
//	
//	if(BSP_W25Qx_Write(wData,0x00,0x1000)== W25Qx_OK)
//		printf(" QSPI Write OK!\r\n");
//	else
//		printf("something wrong in Step3\r\n");
//    /*-Step4- ???  ************************************************Step4*/	
//	if(BSP_W25Qx_Read(rData,0x00,0x1000)== W25Qx_OK)
//		printf(" QSPI Read ok\r\n\r\n");
//	else
//		printf("something wrong in Step4\r\n");
//	
//	printf("QSPI Read Data : \r\n");
//	for(i =0;i<0x1000;i++)
//	{
//		printf("0x%02X  ",rData[i]);
//		if(i%256 == 255)
//			printf("%d\r\n",i);
//	}
//	printf("\r\n%d", i);
//	printf("\r\n\r\n");
//	/*-Step5- ????  ************************************************Step5*/		
//	if(memcmp(wData,rData,0x1000) == 0 ) 
//		printf(" W25Q64FV QuadSPI Test OK\r\n");
//	else
//		printf(" W25Q64FV QuadSPI Test False\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		HAL_Delay(500);
//	for(i =0;i<0x200;i ++)
//	{
//    rData[i] = 0;
//	}
//		BSP_W25Qx_Read(rData,0x00,0x200);
//		for(i =0;i<0x200;i++)
//		HAL_UART_Transmit(&huart1, (const uint8_t *)&rData[i], 1, 1000);
//		printf("\r\ntest\r\n");
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi==(&hspi1))
		;
}
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi==(&hspi1))
		;
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
