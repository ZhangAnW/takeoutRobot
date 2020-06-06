/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "can.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_HEIGHT 400.f
#define MIN_HEIGHT 2.f
#define PinA_H HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET)
#define PinA_L HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET)
#define PinB_H HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET)
#define PinB_L HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET)
#define PinC_H HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_SET)
#define PinC_L HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_RESET)
#define PinD_H HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,GPIO_PIN_SET)
#define PinD_L HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,GPIO_PIN_RESET)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float timems;
extern uint8_t CanReceivedata4[];
int8_t motorCnt = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void CAN_FILTER_INIT(void);
void MotorGoDown(void);
void MotorGoUp(void);
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
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_CAN1_Init();
  /* USER CODE BEGIN 2 */
	/*TIM*/
	__HAL_TIM_ENABLE(&htim2);
	__HAL_TIM_ENABLE_IT(&htim2 , TIM_IT_UPDATE);
	__HAL_TIM_ENABLE(&htim4);
	__HAL_TIM_ENABLE_IT(&htim4 , TIM_IT_UPDATE);
	/*CAN*/
	CAN_FILTER_INIT();
	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);
	HAL_CAN_ActivateNotification(&hcan1,CAN_IT_LAST_ERROR_CODE);
	HAL_CAN_ActivateNotification(&hcan1,CAN_IT_BUSOFF);
	HAL_CAN_ActivateNotification(&hcan1,CAN_IT_ERROR);
	HAL_Delay(5);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if(GetPeirodTimeFlag())
		{
			if(CanReceivedata4[2]>CanReceivedata4[3])//当前楼层大于目标楼层，下楼
				MotorGoDown();
			else if(CanReceivedata4[2]<CanReceivedata4[3])//上楼
				MotorGoUp();
			else 
			{
				PinA_L;
				PinB_L;
				PinC_L;
				PinD_L;
			}
		}
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode 
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void MotorGoUp(void)
{
	switch(motorCnt){
		case 0:
			PinA_H;
			PinB_L;
			PinC_L;
			PinD_L;
			break;
		case 1:
			PinA_H;
			PinB_H;
			PinC_L;
			PinD_L;
			break;
		case 2:
			PinA_L;
			PinB_H;
			PinC_L;
			PinD_L;
			break;
		case 3:
			PinA_L;
			PinB_H;
			PinC_H;
			PinD_L;
			break;
		case 4:
			PinA_L;
			PinB_L;
			PinC_H;
			PinD_L;
			break;
		case 5:
			PinA_L;
			PinB_L;
			PinC_H;
			PinD_H;
			break;
		case 6:
			PinA_L;
			PinB_L;
			PinC_L;
			PinD_H;
			break;
		case 7:
			PinA_H;
			PinB_L;
			PinC_L;
			PinD_H;
			break;
		default :
      PinA_L;
			PinB_L;
			PinC_L;
			PinD_L;
			break;
		}
		motorCnt ++;
		motorCnt = motorCnt%8;
}
void MotorGoDown(void)
{
	switch(motorCnt){
		case 0:
			PinA_H;
			PinB_L;
			PinC_L;
			PinD_L;
			break;
		case 1:
			PinA_H;
			PinB_H;
			PinC_L;
			PinD_L;
			break;
		case 2:
			PinA_L;
			PinB_H;
			PinC_L;
			PinD_L;
			break;
		case 3:
			PinA_L;
			PinB_H;
			PinC_H;
			PinD_L;
			break;
		case 4:
			PinA_L;
			PinB_L;
			PinC_H;
			PinD_L;
			break;
		case 5:
			PinA_L;
			PinB_L;
			PinC_H;
			PinD_H;
			break;
		case 6:
			PinA_L;
			PinB_L;
			PinC_L;
			PinD_H;
			break;
		case 7:
			PinA_H;
			PinB_L;
			PinC_L;
			PinD_H;
			break;
		default :
      PinA_L;
			PinB_L;
			PinC_L;
			PinD_L;
			break;
		}
		motorCnt --;
		if(motorCnt<0)
			motorCnt+=8;
		motorCnt = motorCnt%8;
}
void CAN_FILTER_INIT(void)
{
	CAN_FilterTypeDef filter1;
	filter1.FilterBank=0;
  filter1.FilterMode=CAN_FILTERMODE_IDMASK;
  filter1.FilterScale=CAN_FILTERSCALE_32BIT;
  filter1.FilterIdHigh=0x0000;	
	filter1.FilterIdLow=0x0000;	
	filter1.FilterMaskIdHigh=0x0000;	
	filter1.FilterMaskIdLow=0x0000;
  filter1.FilterFIFOAssignment=CAN_FilterFIFO0;
  filter1.FilterActivation=ENABLE; 
	if(HAL_CAN_ConfigFilter(&hcan1,&filter1)!=HAL_OK)
	{
		Error_Handler(); 
	}
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
