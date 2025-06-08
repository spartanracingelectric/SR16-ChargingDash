/* USER CODE BEGIN Header */
// Written by Ayman Alamayri in Dec 2024
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "display.h"

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
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

CAN_HandleTypeDef hcan1;

I2C_HandleTypeDef hi2c2;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
int selectedButton = 0;
bool backPressed = false;
bool selectPressed = false;
bool isChargerUnsafe = false;

uint32_t CURRENT_TIME = 0;
uint32_t PREVIOUS_TIME = 0;

uint16_t LIMIT_VOLTS = 0;
uint16_t LIMIT_AMPS = 0;

char codeBranch[6] = "Beta";
char codeVersion[5] = "0.3.5";

extern bool isBalancing;
extern bool isBalancingControl;

extern int currentChargingScreen;
extern void SRE_Display_Charging1(void);
extern void SRE_Display_Charging2(void);

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_CAN1_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM3_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

// PRINTF SUPPORT VIA UART - BEGIN
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}
// PRINTF SUPPORT VIA UART - END

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// INTERRUPTS FOR KEYS
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == BTN_UP_Pin || GPIO_Pin == BTN_DWN_Pin || GPIO_Pin == BTN_SEL_Pin || GPIO_Pin == BTN_BCK_Pin) {
  	// TODO: fix debouncing
    CURRENT_TIME = HAL_GetTick();
    int DEB_TIME_THRES = 200;
    int TIME_DIFF = CURRENT_TIME - PREVIOUS_TIME;
    if (TIME_DIFF > DEB_TIME_THRES) {
		if (GPIO_Pin == BTN_UP_Pin && !selectPressed) {
        selectedButton--;
      } else if (GPIO_Pin == BTN_DWN_Pin && !selectPressed) {
        selectedButton++;
      } else if (GPIO_Pin == BTN_SEL_Pin) {
        selectPressed = true;
      } else if (GPIO_Pin == BTN_BCK_Pin && !selectPressed) {
        backPressed = true;
      }
      PREVIOUS_TIME = CURRENT_TIME;
    }
  }
}

// FAN SPEED CONTROL
void FAN_SPD_CTRL(uint32_t fan_speed) {
  if (fan_speed <= 100) {
    TIM3->CCR1 = fan_speed;
  } else {
    // TODO: Throw error
  }
}

// READ THERMISTOR VALUE
float READ_THERM(uint16_t* adc_thermistor, uint16_t therm_first_resistance) {
  float calibration_a = 1.462805229e-3;
  float calibration_b = 2.310582766e-4;
  float calibration_c = 1.189005910e-7;
  float thermistor_voltage = (*adc_thermistor / 4095.0) * 3.3;
  float therm_inlet_resistance = (thermistor_voltage * therm_first_resistance) / (3.3 - thermistor_voltage);
  float temperature = (1 / (calibration_a + calibration_b * log(therm_inlet_resistance) + calibration_c * pow(log(therm_inlet_resistance), 3))) - 273.15;
  return temperature;
}

// SEND i2c to Atiny for SOC
void NEOPIX_CTRL(int SOC) {
}

// READ SHUNT
void READ_SHUNT() {
}

// READ CONTROL PILOT
void READ_CPILOT() {
}

// CAN STUFF BEGIN

struct CANMessage {
	CAN_TxHeaderTypeDef TxHeader;
	uint32_t TxMailbox;
	uint8_t data[8];
};

struct bmsAndElconData {
  float BMS_avgVolt;
  float BMS_minVolt;
  float BMS_maxVolt;
  float BMS_avgTemp;
  float BMS_minTemp;
  float BMS_maxTemp;
  float BMS_stateOfCharge;
  float BMS_packImbalance;
  float ELCON_outVolt;
  float ELCON_outCurrent;
  bool ELCON_fault[5];
  /*
    Bit 0: 0 -> no hw fail, 1 -> hw fail
    Bit 1: 0 -> no over temp, 1 -> overtemp
    Bit 2: 0 -> input volt right, 1 -> input volt wrong
    Bit 3: 0 -> batt volt detected, 1 -> batt volt not detected
    Bit 4: 0 -> comms good, 1 -> comms timeout
  */
};

// TODO: maybe move the below
struct bmsAndElconData currentBmsAndElconData = {0};
CAN_RxHeaderTypeDef RxHeader;
uint8_t RxData[8];

uint32_t elconBmsFilterIDs[4] = {
  0x18FF50E5, // Elcon
  0x622, // BMS volt/temp
  0x621, // BMS soc
  0x600, // BMS imbalance
};
// TODO: maybe move the above

HAL_StatusTypeDef CAN_Start() {
	return HAL_CAN_Start(&hcan1);
}

HAL_StatusTypeDef CAN_Activate() {
	return HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

HAL_StatusTypeDef CAN_Send(struct CANMessage *canMsgPtr) {
	return HAL_CAN_AddTxMessage(&hcan1, &canMsgPtr->TxHeader, (uint8_t*) canMsgPtr->data, &canMsgPtr->TxMailbox);
}

void CAN_SettingsInit(struct CANMessage *canMsgPtr, bool isExtended, uint16_t dlc_length, uint32_t filterIDs[4]) {
	CAN_Start();
	CAN_Activate();

	canMsgPtr->TxHeader.IDE = (isExtended) ? CAN_ID_EXT : CAN_ID_STD;
  canMsgPtr->TxHeader.ExtId = (isExtended) ? 0x00000000 : 0x000;
	canMsgPtr->TxHeader.RTR = CAN_RTR_DATA;
	canMsgPtr->TxHeader.DLC = dlc_length;

  // Filter 1: First two IDs
  CAN_FilterTypeDef filterConfig1 = {0};
  filterConfig1.FilterBank = 18;
  filterConfig1.FilterMode = CAN_FILTERMODE_IDLIST;
  filterConfig1.FilterScale = CAN_FILTERSCALE_32BIT;
  filterConfig1.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  filterConfig1.FilterActivation = ENABLE;
  filterConfig1.FilterIdHigh = (filterIDs[0] >> 13) & 0xFFFF;
  filterConfig1.FilterIdLow  = (filterIDs[0] << 3) & 0xFFFF;
  filterConfig1.FilterMaskIdHigh = (filterIDs[1] >> 13) & 0xFFFF;
  filterConfig1.FilterMaskIdLow  = (filterIDs[1] << 3) & 0xFFFF;
  HAL_CAN_ConfigFilter(&hcan1, &filterConfig1);

  // Filter 2: Next two IDs
  CAN_FilterTypeDef filterConfig2 = {0};
  filterConfig2.FilterBank = 19;
  filterConfig2.FilterMode = CAN_FILTERMODE_IDLIST;
  filterConfig2.FilterScale = CAN_FILTERSCALE_32BIT;
  filterConfig2.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  filterConfig2.FilterActivation = ENABLE;
  filterConfig2.FilterIdHigh = (filterIDs[2] >> 13) & 0xFFFF;
  filterConfig2.FilterIdLow  = (filterIDs[2] << 3) & 0xFFFF;
  filterConfig2.FilterMaskIdHigh = (filterIDs[3] >> 13) & 0xFFFF;
  filterConfig2.FilterMaskIdLow  = (filterIDs[3] << 3) & 0xFFFF;
  HAL_CAN_ConfigFilter(&hcan1, &filterConfig2);

  HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void Set_CAN_Id(struct CANMessage *ptr, uint32_t id, bool isExtended) {
  if (isExtended) {
	  ptr->TxHeader.ExtId = id;
  } else {
    ptr->TxHeader.StdId = id;
  }
}

void CAN_Balance(struct CANMessage *ptr, bool balancing_enabled) {
  uint32_t CAN_ID = 0x604;
  Set_CAN_Id(ptr, CAN_ID, false);
  ptr->data[0] = (balancing_enabled) ? 0x1 : 0x0;
  HAL_Delay(10);
  CAN_Send(ptr);
}

void CAN_Charge(struct CANMessage *ptr, uint16_t chargingLimitsVolts, uint16_t chargingLimitsAmps, bool charge_enable) {
  // Check for mailbox instead of delay
  uint32_t CAN_ID = 0x1806E5F4;
  Set_CAN_Id(ptr, CAN_ID, true);

  chargingLimitsVolts *= 10;
  chargingLimitsAmps *= 10;

  ptr->data[0] = (chargingLimitsVolts >> 8) & 0xFF;
  ptr->data[1] = chargingLimitsVolts & 0xFF;
  ptr->data[2] = (chargingLimitsAmps >> 8) & 0xFF;
  ptr->data[3] = chargingLimitsAmps & 0xFF;
  ptr->data[4] = (charge_enable) ? 0x00 : 0x01;
  ptr->data[5] = 0x00;
  ptr->data[6] = 0x00;
  ptr->data[7] = 0x00;

  HAL_Delay(3);
  CAN_Send(ptr);
}

// struct outputData
// uint32_t filterIDs[4]
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK) {
    Error_Handler();
  }
  // TODO only check every 1 second
  if (RxHeader.ExtId == elconBmsFilterIDs[0]) {
    currentBmsAndElconData.ELCON_outVolt = RxData[0] + RxData[1];
    currentBmsAndElconData.ELCON_outCurrent = RxData[2] + RxData[3];
    /*
      Bit 0: 0 -> no hw fail, 1 -> hw fail
      Bit 1: 0 -> no over temp, 1 -> overtemp
      Bit 2: 0 -> input volt right, 1 -> input volt wrong
      Bit 3: 0 -> batt volt detected, 1 -> batt volt not detected
      Bit 4: 0 -> comms good, 1 -> comms timeout
    */
    currentBmsAndElconData.ELCON_fault[0] = (RxData[4] && 0xF0000) ? true : false; // TODO: figure out
    currentBmsAndElconData.ELCON_fault[1] = (RxData[4] && 0x0F000) ? true : false; // TODO: figure out
    currentBmsAndElconData.ELCON_fault[2] = (RxData[4] && 0x00F00) ? true : false; // TODO: figure out
    currentBmsAndElconData.ELCON_fault[3] = (RxData[4] && 0x000F0) ? true : false; // TODO: figure out
    currentBmsAndElconData.ELCON_fault[4] = (RxData[4] && 0x0000F) ? true : false; // TODO: figure out
    printf("Received bms temp packet.\n");
  } else if (RxHeader.StdId == elconBmsFilterIDs[1]) {
    currentBmsAndElconData.BMS_minVolt = RxData[0]; // TODO: figure out
    currentBmsAndElconData.BMS_maxVolt = RxData[1]; // TODO: figure out
    currentBmsAndElconData.BMS_minTemp = RxData[2]; // TODO: figure out
    currentBmsAndElconData.BMS_maxTemp = RxData[3]; // TODO: figure out
    currentBmsAndElconData.BMS_avgVolt = 0; // TODO: figure out
    currentBmsAndElconData.BMS_avgTemp = 0; // TODO: figure out
    printf("Received elcon packet.\n");
  } else if (RxHeader.StdId == elconBmsFilterIDs[2]) {
    currentBmsAndElconData.BMS_stateOfCharge = RxData[2]; // TODO: figure out
    printf("Received bms soc packet.\n");
  } else if (RxHeader.StdId == elconBmsFilterIDs[3]) {
    currentBmsAndElconData.BMS_packImbalance = RxData[9]; // TODO: figure out
    printf("Received bms imbalance packet.\n");
  }
  // From suguru: Use sum of cell???
}

// CAN STUFF END

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
  MX_CAN1_Init();
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  // INIT DISPLAY
  SRE_Display_Init(false);

  // INIT CHARGING CAN STRUCT
  struct CANMessage charging_msg;
  CAN_SettingsInit(&charging_msg, true, 8, elconBmsFilterIDs);

  // INIT BALANCING CAN STRUCT
  struct CANMessage balancing_msg;
  CAN_SettingsInit(&balancing_msg, false, 1, elconBmsFilterIDs);

  // INIT PWM
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

  // INIT ADC
  uint16_t adc_buffer[2];
  HAL_TIM_Base_Start(&htim3);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, 2);

  HAL_Delay(500);

  // TEMP STUFF 1 START
  DISP_KanoaSplash(); // TODO: call this in the GUI init instead
  HAL_Delay(1000);
  FAN_SPD_CTRL(100); // TODO: make this based on temp
  uint16_t therm_inlet = adc_buffer[0];
  uint16_t therm_outlet = adc_buffer[1];
  GPIO_PinState IN_HVIL_SW_STATE;
  GPIO_PinState RTC_SW_STATE;
  GPIO_PinState IN_HVIL_ESTOP_Pin_State;
  GPIO_PinState IN_HVIL_CHAR_Pin_State;
  GPIO_PinState IN_HVIL_TERM_Pin_State;
  GPIO_PinState IN_HVIL_ACUM_Pin_State;
  GPIO_PinState IN_HVIL_FSW_Pin_State;
  char chargingInfoString[30];
  SRE_Display_Test(); // TODO: better init for GUI
  // TEMP STUFF 1 END

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // TODO: CHECK ALL LEDS AND PERIPHERALS WORK
	  ssd1306_Fill(Black);
	  //ssd1306_UpdateScreen();

    IN_HVIL_SW_STATE = HAL_GPIO_ReadPin(IN_HVIL_FSW_GPIO_Port, IN_HVIL_FSW_Pin);
	  RTC_SW_STATE = HAL_GPIO_ReadPin(IN_RTC_SW_GPIO_Port, IN_RTC_SW_Pin);

    // TEMP STUFF 2 START
    sprintf(chargingInfoString, "%d volts @ %d amps", LIMIT_VOLTS, LIMIT_AMPS);
    // TEMP STUFF 2 END


    IN_HVIL_ESTOP_Pin_State = HAL_GPIO_ReadPin(IN_HVIL_ESTOP_GPIO_Port, IN_HVIL_ESTOP_Pin);
    IN_HVIL_TERM_Pin_State = HAL_GPIO_ReadPin(IN_HVIL_TERM_GPIO_Port, IN_HVIL_TERM_Pin);
    IN_HVIL_ACUM_Pin_State = HAL_GPIO_ReadPin(IN_HVIL_ACUM_GPIO_Port, IN_HVIL_ACUM_Pin);
    IN_HVIL_FSW_Pin_State = HAL_GPIO_ReadPin(IN_HVIL_FSW_GPIO_Port, IN_HVIL_FSW_Pin);

    if (IN_HVIL_ESTOP_Pin_State == GPIO_PIN_SET ||
        IN_HVIL_CHAR_Pin_State  == GPIO_PIN_SET ||
        IN_HVIL_TERM_Pin_State  == GPIO_PIN_SET ||
        IN_HVIL_ACUM_Pin_State  == GPIO_PIN_SET)
        isChargerUnsafe = true;
    else if (IN_HVIL_ESTOP_Pin_State == GPIO_PIN_RESET &&
        IN_HVIL_CHAR_Pin_State  == GPIO_PIN_RESET &&
        IN_HVIL_TERM_Pin_State  == GPIO_PIN_RESET &&
        IN_HVIL_ACUM_Pin_State  == GPIO_PIN_RESET)
        isChargerUnsafe = false;

    // TODO: add -> IN_HVIL_CHAR_Pin_State  == GPIO_PIN_RESET

    if (!isChargerUnsafe)
    {
      CAN_Charge(&charging_msg, LIMIT_VOLTS, LIMIT_AMPS, false);
      ssd1306_Fill(Black);
	    ssd1306_UpdateScreen();
      ssd1306_SetCursor(5, 5);
      ssd1306_WriteString("HVIL ERROR", Font_6x8, White); // TODO: make it more clear
      ssd1306_UpdateScreen();
    } else if (isChargerUnsafe && IN_HVIL_FSW_Pin_State) {
      if(RTC_SW_STATE) {
        HAL_GPIO_WritePin(GPIOA, LED_HV_Pin, GPIO_PIN_SET);
        ssd1306_SetCursor(5, 5);
        CAN_Charge(&charging_msg, LIMIT_VOLTS, LIMIT_AMPS, false);
        ssd1306_WriteString("PLS FLIP RTC", Font_6x8, White);
      } else if (!RTC_SW_STATE) {
        CAN_Charge(&charging_msg, LIMIT_VOLTS, LIMIT_AMPS, true);
        ssd1306_WriteString("Now Charging", Font_6x8, White);
        ssd1306_SetCursor(5, 20);
        ssd1306_WriteString(chargingInfoString, Font_6x8, White);
      } else if (isBalancingControl) {
          CAN_Balance(&balancing_msg, isBalancing);
          HAL_GPIO_WritePin(GPIOA, LED_BAL_Pin, GPIO_PIN_SET);
          ssd1306_SetCursor(5, 5);
          ssd1306_WriteString("Now Balancing", Font_6x8, White);
      } else {
        HAL_GPIO_WritePin(GPIOA, LED_HV_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, LED_BAL_Pin, GPIO_PIN_RESET);
        ssd1306_SetCursor(5, 5);
        ssd1306_WriteString("Not Charging", Font_6x8, White);
      }
    }
		ssd1306_UpdateScreen();
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV5;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_PLL2;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL2_ON;
  RCC_OscInitStruct.PLL2.PLL2MUL = RCC_PLL2_MUL8;
  RCC_OscInitStruct.PLL2.HSEPrediv2Value = RCC_HSE_PREDIV2_DIV5;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the Systick interrupt time
  */
  __HAL_RCC_PLLI2S_ENABLE();
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 2;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  hcan1.Init.Prescaler = 9;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_3TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 72-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 100-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 72-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 100-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
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

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_HV_Pin|LED_BAL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_ELCON_FLT_GPIO_Port, LED_ELCON_FLT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(HVIL_CTRL_GPIO_Port, HVIL_CTRL_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : IN_HVIL_ACUM_Pin IN_HVIL_FSW_Pin IN_HVIL_ESTOP_Pin */
  GPIO_InitStruct.Pin = IN_HVIL_ACUM_Pin|IN_HVIL_FSW_Pin|IN_HVIL_ESTOP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_HV_Pin LED_BAL_Pin */
  GPIO_InitStruct.Pin = LED_HV_Pin|LED_BAL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : IN_BMS_FLT_LED_Pin IN_IMD_FLT_LED_Pin IN_RTC_SW_Pin */
  GPIO_InitStruct.Pin = IN_BMS_FLT_LED_Pin|IN_IMD_FLT_LED_Pin|IN_RTC_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_ELCON_FLT_Pin */
  GPIO_InitStruct.Pin = LED_ELCON_FLT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_ELCON_FLT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_TSAL_FLT_Pin */
  GPIO_InitStruct.Pin = LED_TSAL_FLT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LED_TSAL_FLT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : IN_HVIL_TERM_Pin */
  GPIO_InitStruct.Pin = IN_HVIL_TERM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(IN_HVIL_TERM_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : HVIL_CTRL_Pin */
  GPIO_InitStruct.Pin = HVIL_CTRL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(HVIL_CTRL_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BTN_UP_Pin BTN_DWN_Pin BTN_SEL_Pin */
  GPIO_InitStruct.Pin = BTN_UP_Pin|BTN_DWN_Pin|BTN_SEL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : BTN_BCK_Pin */
  GPIO_InitStruct.Pin = BTN_BCK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BTN_BCK_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

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
