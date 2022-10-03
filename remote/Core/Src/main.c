/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery.h"
#include "../../Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_lcd.h"
#include "../../Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_ts.h"
#include "IRremote.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define TS_EVENT_FLAG_MASK 0x00000001U
#define FORWARD 0x00U
#define BACKWARD 0x01U
#define LEFT 0x02U
#define RIGHT 0x03U

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */

osMessageQueueId_t qid_IRqueue;
osEventFlagsId_t eid_EventFlags;    // event flags (signals)
uint8_t  lcd_status = LCD_OK;
uint32_t ts_status = TS_OK;
TS_StateTypeDef TS_State = {0};
uint16_t xt;
uint16_t yt;

osThreadId_t TouchScreenTaskHandle;
const osThreadAttr_t TouchScreenTask_attributes = {
		  .name = "TouchScreen",
		  .priority = (osPriority_t) osPriorityNormal,
		  .stack_size = 256
};

osThreadId_t InfraRedSendTaskHandle;
const osThreadAttr_t InfraRedSendTask_attributes = {
		  .name = "InfraRed",
		  .priority = (osPriority_t) osPriorityNormal,
		  .stack_size = 256
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void StartDefaultTask(void *argument);

/* USER CODE BEGIN PFP */

void TouchScreenTask(void *argument);
void InfraRedSendTask(void *argument);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void begin()
{
	/* Initialize LEDs */
	BSP_LED_Init(LED_RED);
	BSP_LED_Init(LED_GREEN);

	/* Initialize the LCD */
	lcd_status = BSP_LCD_Init();
	while(lcd_status != LCD_OK);
	BSP_LCD_LayerDefaultInit(LTDC_DEFAULT_ACTIVE_LAYER, LCD_FB_START_ADDRESS);
	BSP_LCD_SelectLayer(LTDC_DEFAULT_ACTIVE_LAYER);
	BSP_LCD_DisplayOn();
	BSP_LCD_Clear(LCD_COLOR_WHITE);

	BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
	BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize() * 0.25, BSP_LCD_GetYSize());
	BSP_LCD_FillRect(BSP_LCD_GetXSize() * 0.75, 0, BSP_LCD_GetXSize() * 0.25, BSP_LCD_GetYSize());
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_FillRect(BSP_LCD_GetXSize() * 0.25, 0, BSP_LCD_GetXSize() * 0.5, BSP_LCD_GetYSize() * 0.5);
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_FillRect(BSP_LCD_GetXSize() * 0.25, BSP_LCD_GetYSize() * 0.5, BSP_LCD_GetXSize() * 0.5, BSP_LCD_GetYSize() * 0.5);

	// Init Touch Screen
	ts_status = BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
	while(ts_status != TS_OK);
	// Configure and enable the touch screen interrupts:
	ts_status = BSP_TS_ITConfig();
	while(ts_status != TS_OK);

	return;
}

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

  /* USER CODE BEGIN 2 */

  begin();
  IRsend_enableIROut(38);

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  qid_IRqueue = osMessageQueueNew(256, sizeof(uint8_t), NULL); //vrsta mora biti dovolj velika drugace je hardfault error
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  //defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  TouchScreenTaskHandle = osThreadNew(TouchScreenTask, NULL, &TouchScreenTask_attributes);
  InfraRedSendTaskHandle = osThreadNew(InfraRedSendTask, NULL, &InfraRedSendTask_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  eid_EventFlags = osEventFlagsNew(NULL);
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

//FROM: example lcd
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 200000000
  *            HCLK(Hz)                       = 200000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 400
  *            PLL_P                          = 2
  *            PLL_Q                          = 4 ////8
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 6
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();////
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);////

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  ////RCC_OscInitStruct.PLL.PLLQ = 8;
  ////RCC_OscInitStruct.PLL.PLLR = 7;
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

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

//After a touch is detected, the corresponding command is stored into qid_IRqueue queue
void TouchScreenTask(void *argument)
{
  uint32_t flags;
  osStatus_t status;
  uint8_t message;

  /* Infinite loop */
  for(;;)
  {
    flags = osEventFlagsWait(eid_EventFlags, TS_EVENT_FLAG_MASK, osFlagsWaitAny, osWaitForever);
    while (flags < 0) {}; //error
	//handle event:
	BSP_LED_Toggle(LED_RED);

	status = BSP_TS_GetState(&TS_State);
	while(status != osOK);

	if(TS_State.touchDetected == 1)
	{
	  // Get X and Y position of the first
	  xt = TS_State.touchX[0];
	  yt = TS_State.touchY[0];

	  if (xt < (uint16_t) (BSP_LCD_GetXSize() * 0.25)) //left
		  message = LEFT;
	  else if (xt > (uint16_t) (BSP_LCD_GetXSize() * 0.75)) //right
		  message = RIGHT;
	  else if (yt < (uint16_t) (BSP_LCD_GetYSize() * 0.5)) //forward
		  message = FORWARD;
	  else //backward
		  message = BACKWARD;

	  if (osMessageQueueGetCount(qid_IRqueue) != osMessageQueueGetCapacity(qid_IRqueue))
		  status = osMessageQueuePut(qid_IRqueue, &message, 0, osWaitForever);
	  while(status != osOK);

	  osDelay(30); //so that the queue does not fills up
	}
  }
  return;
}

//Every 100ms the queue length is checked and messages are sent trough the IR transmitter (SIRC SONY protocol) with high priority
void InfraRedSendTask(void *argument)
{
	osStatus_t status;
	uint8_t message;
	for (;;)
	{
		if (osMessageQueueGetCount(qid_IRqueue))
		{
			status = osThreadSetPriority(InfraRedSendTaskHandle, (osPriority_t) osPriorityHigh);
			while(status != osOK);
			status = osMessageQueueGet(qid_IRqueue, &message, NULL, osWaitForever);
			while(status != osOK);
			// a4 a3 a2 a1 a0 c6 c5 c4 c3 c2 c1 c0
			IRsend_sendSony(message | 0x000, 12);
			osDelay(30); //delay between sending messages
		}
		else
		{
			status = osThreadSetPriority(InfraRedSendTaskHandle, (osPriority_t) osPriorityNormal);
			osDelay(100);
		}
	}
	return;
}

/**
  * @brief  EXTI line detection callbacks
  * @param  GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) //funkcija zajame vse exti prekinitve, locimo jih po pinu
{

  switch(GPIO_Pin)
  	{
  	  //case USER_BUTTON_PIN: // USER_BUTTON_PIN (GPIO_PIN_0) is the Blue Button
  		//BSP_LCD_Clear(LCD_COLOR_WHITE);
  		//break;
  	  case TS_INT_PIN: // TS_INT_PIN (GPIO_PIN_13) is the Touch Screen Interrupt Request
  		osEventFlagsSet(eid_EventFlags, TS_EVENT_FLAG_MASK); // send signal to a thread
  		break;
  	}

}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	//IRsend_sendSony(0x001, 12);
    osDelay(1000);
  }
  /* USER CODE END 5 */
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
