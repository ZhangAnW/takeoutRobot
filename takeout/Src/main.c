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
#include "usart.h"
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
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//---can-----
#define CAN_Id_Standard             ((uint32_t)0x00000000)  /*!< Standard Id */
#define CAN_Id_Extended             ((uint32_t)0x00000004)  /*!< Extended Id */
void CAN_TxHeader_Init(CAN_TxHeaderTypeDef    * pHeader,
                                                        uint32_t                             StdId);
uint8_t CanSendMsg[4] = {0};
uint32_t DLC;//length
CAN_TxHeaderTypeDef hCAN1_TxHeader; //CAN1发送消息
CAN_RxHeaderTypeDef hCAN1_RxHeader; //CAN1接收消息
CAN_FilterTypeDef hCAN1_Filter; //CAN1滤波器

uint8_t triggerFlag=0;
uint8_t triggerCntFlag=0;
uint8_t echoFlag=0;
uint8_t echoCntFlag=0;
char nowFloor=0,targetFloor=0;//当前楼层和目标楼层
char USART_temp[5]={0};

float height;
float timems;
extern uint8_t triggerCnt;
extern uint16_t echoCnt;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void DataSend(void);
void CalculateHeight(void);
void WaitForEcho(void);
void TriggerAction(void);
void CAN_FILTER_INIT(void);
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
  MX_CAN1_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	/*UART*/
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
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
		if(height <10.f)
			nowFloor =0;
		else if(height < 50.f)
			nowFloor = 1;
		else if(height < 100.f)
			nowFloor = 2;
		else if(height < 150.f)
			nowFloor = 3;
		else if(height < 200.f)
			nowFloor = 4;
		else if(height < 250.f)
			nowFloor = 5;
		else if(height < 300.f)
			nowFloor = 6;
		else if(height < 350.f)
			nowFloor = 7;
		else if(height < 400.f)
			nowFloor = 8;
		else nowFloor = 9;
		if(strcmp(USART_temp,"01")==0)
		{
			targetFloor = 1;
		}
		else if(strcmp(USART_temp,"02")==0)
		{
			targetFloor = 2;
		}
		else if(strcmp(USART_temp,"03")==0)
		{
			targetFloor = 3;
		}
		else if(strcmp(USART_temp,"04")==0)
		{
			targetFloor = 4;
		}
		else if(strcmp(USART_temp,"05")==0)
		{
			targetFloor = 5;
		}
		else if(strcmp(USART_temp,"06")==0)
		{
			targetFloor = 6;
		}
		else if(strcmp(USART_temp,"07")==0)
		{
			targetFloor = 7;
		}
		else if(strcmp(USART_temp,"08")==0)
		{
			targetFloor = 8;
		}
		else if(strcmp(USART_temp,"09")==0)
		{
			targetFloor = 9;
		}
		else if(strcmp(USART_temp,"10")==0)
		{
			targetFloor = 10;
		}
		if(GetPeirodTimeFlag())
		{
			TriggerAction();
			WaitForEcho();
			if(echoFlag)
			{
				CalculateHeight();
//				DataSend();
				echoCnt=0;
				echoFlag=0;
				triggerFlag=0;
			}
		}
		CanSendMsg[0] = 'A';
		CanSendMsg[1] = 'T';
		CanSendMsg[2] = nowFloor;
		CanSendMsg[3] = targetFloor;
		DLC = 4;
		CAN_TxHeader_Init(&hCAN1_TxHeader,1);
		HAL_CAN_AddTxMessage(&hcan1,&hCAN1_TxHeader,CanSendMsg,&DLC);
		CAN_TxHeader_Init(&hCAN1_TxHeader,2);
		HAL_CAN_AddTxMessage(&hcan1,&hCAN1_TxHeader,CanSendMsg,&DLC);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
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
//void DataSend(void)
//{
//	HAL_UART_Transmit(&huart2,(uint8_t*)"Height: ",8,5);
//	USART_Out_Float(&huart2,height);
//	HAL_UART_Transmit(&huart2,(uint8_t*)"\t\t",2,5);
//	HAL_UART_Transmit(&huart2,(uint8_t*)"Time: ",6,5);
//	USART_Out_Float(&huart2,timems);
//	HAL_UART_Transmit(&huart2,(uint8_t*)"\r\n",2,5);
//}
void CalculateHeight(void)
{
	height=echoCnt*34000/(100000.f*2);	//echoCnt单位为10us
	timems=echoCnt/100.f;
	if(height>=MAX_HEIGHT)
	{
		height=MAX_HEIGHT;
	}
	if(height<=MIN_HEIGHT)
	{
		height=MIN_HEIGHT;
	}
}
void WaitForEcho(void)
{
	while(HAL_GPIO_ReadPin(ECHO_GPIO_Port,ECHO_Pin)==0){}
	echoCntFlag=1;
	while(HAL_GPIO_ReadPin(ECHO_GPIO_Port,ECHO_Pin)==1){}
	echoCntFlag=0;
	echoFlag=1;
}
void TriggerAction(void)
{
	if(!triggerFlag)
	{
		triggerCntFlag=1;
		for(;GetTriggerTimeFlag()<1;)
		{
			HAL_GPIO_WritePin(TRIG_GPIO_Port,TRIG_Pin,GPIO_PIN_SET);
			triggerFlag=1;
		}
		HAL_GPIO_WritePin(TRIG_GPIO_Port,TRIG_Pin,GPIO_PIN_RESET);
		triggerCntFlag=0;

	}
}
void CAN_TxHeader_Init(CAN_TxHeaderTypeDef    * pHeader,
                                                        uint32_t                             StdId)
{
    pHeader->StdId    = StdId;    //11位     标准标识符
    pHeader->ExtId    = StdId;    //29位     扩展标识符
    pHeader->IDE        = 0;        //1位        0:标准帧 1:拓展帧
    pHeader->RTR        = 0;      //1位   0:数据帧 1:远程帧
    pHeader->DLC        = 4;        //4位   发送的数据的长度
    pHeader->TransmitGlobalTime    =    ENABLE;
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
