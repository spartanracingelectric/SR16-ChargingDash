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
#define FLAG_600 (1 << 0)
#define FLAG_621 (1 << 1)
#define FLAG_622 (1 << 2)

// TODO: move stuff here
#define UPPER_MAX_CELL_CV_THRESH 4.25 // Competition
#define LOWER_MAX_CELL_CV_THRESH 4.1
#define MIN_ALLOWED_IMBAL 0.01
#define MAX_ALLOWED_PWR 4000
#define MAINT_AMPS 0.5

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
bool isChargerSafe = false;
bool isCharging = false;
extern bool isChargingSequence;

uint32_t CURRENT_TIME = 0;
uint32_t PREVIOUS_TIME = 0;

uint16_t LIMIT_VOLTS = 0;
uint16_t LIMIT_AMPS = 0;

uint16_t THERM_RESIST = 12000;
uint16_t *therm_inlet = NULL;
uint16_t *therm_outlet = NULL;

float AMPS_AT_LOWER_MAX_CELL_CV_THRESH = 0;

char codeBranch[6] = "Beta";
char codeVersion[5] = "0.3.X";

//TODO: CHECK IF BMS IS ACTUALLY BALANCING
extern bool isBalancing;
extern bool isBalancingControl;

extern int currentChargingScreen;
extern void SRE_Display_Charging1(void);
extern void SRE_Display_Charging2(void);

static uint8_t bmsFlags = 0;

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


// TODO: fix
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

// TODO: complete
// SEND i2c to Atiny for SOC
void NEOPIX_CTRL(int SOC) {
}

// TODO: complete
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
  float BMS_sumOfCells;
  float BMS_minVolt;
  float BMS_maxVolt;
  float BMS_avgTemp;
  float BMS_minTemp;
  float BMS_maxTemp;
  float BMS_stateOfCharge; // TODO: not float
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
volatile struct bmsAndElconData currentBmsAndElconData = {0};
CAN_RxHeaderTypeDef RxHeader;
uint8_t RxData[8];

uint32_t elconBmsFilterIDs[4] = {
  0x600, // BMS imbalance
  0x621, // BMS soc
  0x622, // BMS volt/temp high+low
  0x18FF50E5, // Elcon
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

void CAN_SettingsInit(struct CANMessage *canMsgPtr, bool isExtended, uint16_t dlc_length) {
  CAN_Start();
  CAN_Activate();

  canMsgPtr->TxHeader.IDE = (isExtended) ? CAN_ID_EXT : CAN_ID_STD;
  canMsgPtr->TxHeader.ExtId = (isExtended) ? 0x00000000 : 0x000;
  canMsgPtr->TxHeader.RTR = CAN_RTR_DATA;
  canMsgPtr->TxHeader.DLC = dlc_length;

  // ----- Filter 0: BMS ID 0x600 -----
  CAN_FilterTypeDef filter0 = {0};
  filter0.FilterBank = 0;
  filter0.FilterMode = CAN_FILTERMODE_IDMASK;
  filter0.FilterScale = CAN_FILTERSCALE_32BIT;
  filter0.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  filter0.FilterActivation = ENABLE;
  filter0.FilterIdHigh     = (elconBmsFilterIDs[0] << 5) & 0xFFFF;
  filter0.FilterIdLow      = 0;
  filter0.FilterMaskIdHigh = 0xFFFF;
  filter0.FilterMaskIdLow  = 0xFFFF;

  // ----- Filter 1: BMS ID 0x621 -----
  CAN_FilterTypeDef filter1 = {0};
  filter1.FilterBank = 1;
  filter1.FilterMode = CAN_FILTERMODE_IDMASK;
  filter1.FilterScale = CAN_FILTERSCALE_32BIT;
  filter1.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  filter1.FilterActivation = ENABLE;
  filter1.FilterIdHigh     = (elconBmsFilterIDs[1] << 5) & 0xFFFF;
  filter1.FilterIdLow      = 0;
  filter1.FilterMaskIdHigh = 0xFFFF;
  filter1.FilterMaskIdLow  = 0xFFFF;

  // ----- Filter 2: BMS ID 0x622 -----
  CAN_FilterTypeDef filter2 = {0};
  filter2.FilterBank = 2;
  filter2.FilterMode = CAN_FILTERMODE_IDMASK;
  filter2.FilterScale = CAN_FILTERSCALE_32BIT;
  filter2.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  filter2.FilterActivation = ENABLE;
  filter2.FilterIdHigh     = (elconBmsFilterIDs[2] << 5) & 0xFFFF;
  filter2.FilterIdLow      = 0;
  filter2.FilterMaskIdHigh = 0xFFFF;
  filter2.FilterMaskIdLow  = 0xFFFF;

  // ----- Filter 3: ELCON Extended ID 0x18FF50E5 -----
  CAN_FilterTypeDef filter3 = {0};
  filter3.FilterBank = 3;
  filter3.FilterMode = CAN_FILTERMODE_IDMASK;
  filter3.FilterScale = CAN_FILTERSCALE_32BIT;
  filter3.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  filter3.FilterActivation = ENABLE;
  filter3.FilterIdHigh     = (elconBmsFilterIDs[3] >> 13) & 0xFFFF;
  filter3.FilterIdLow      = ((elconBmsFilterIDs[3] << 3) & 0xFFFF) | (1 << 2);
  filter3.FilterMaskIdHigh = 0xFFFF;
  filter3.FilterMaskIdLow  = 0xFFFF;

  // Apply filters
  HAL_CAN_ConfigFilter(&hcan1, &filter0);
  HAL_CAN_ConfigFilter(&hcan1, &filter1);
  HAL_CAN_ConfigFilter(&hcan1, &filter2);
  HAL_CAN_ConfigFilter(&hcan1, &filter3);

  HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void Set_CAN_Id(struct CANMessage *ptr, uint32_t id, bool isExtended) {
  if (isExtended) {
	  ptr->TxHeader.ExtId = id;
  } else {
    ptr->TxHeader.StdId = id;
  }
}

void printBmsAndElconData(const volatile struct bmsAndElconData *d) {
    printf("BMS_avgVolt       = %f V\n", d->BMS_avgVolt);
    printf("BMS_sumOfCells    = %fV\n", d->BMS_sumOfCells);
    printf("BMS_minVolt       = %f V\n", d->BMS_minVolt);
    printf("BMS_maxVolt       = %f V\n", d->BMS_maxVolt);
    printf("BMS_minTemp       = %f °C\n", d->BMS_minTemp);
    printf("BMS_maxTemp       = %f °C\n", d->BMS_maxTemp);
    printf("BMS_stateOfCharge = %f %%\n", d->BMS_stateOfCharge);
    printf("BMS_packImbalance = %f V\n", d->BMS_packImbalance);
    printf("ELCON_outVolt     = %f V\n", d->ELCON_outVolt);
    printf("ELCON_outCurrent  = %f A\n", d->ELCON_outCurrent);

    // Fault bits: 0=hw fail, 1=overtemp, 2=input volt wrong, 3=batt volt not detected, 4=comms timeout
    printf("ELCON_faults      = [");
    for (int i = 0; i < 5; ++i) {
        printf("%s", d->ELCON_fault[i] ? "1" : "0");
        if (i < 4) printf(", ");
    }
    printf("]\n");
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK) {
    Error_Handler();
  }

  CURRENT_TIME = HAL_GetTick();
  int BMS_CAN_DEBOUNCE_MS = 1000;
  int TIME_DIFF = CURRENT_TIME - PREVIOUS_TIME;

  if (RxHeader.IDE == CAN_ID_EXT && RxHeader.ExtId == elconBmsFilterIDs[3]) {
    currentBmsAndElconData.ELCON_outVolt = ((RxData[0] * 100.0) + RxData[1]) / 10.0;
    currentBmsAndElconData.ELCON_outCurrent = RxData[3] / 10.0;
    currentBmsAndElconData.ELCON_fault[4] = RxData[4] & 0x10;
    currentBmsAndElconData.ELCON_fault[3] = RxData[4] & 0x08;
    currentBmsAndElconData.ELCON_fault[2] = RxData[4] & 0x04;
    currentBmsAndElconData.ELCON_fault[1] = RxData[4] & 0x02;
    currentBmsAndElconData.ELCON_fault[0] = RxData[4] & 0x01;
  }

  if (RxHeader.IDE == CAN_ID_STD && TIME_DIFF > BMS_CAN_DEBOUNCE_MS) {
    if (RxHeader.StdId == elconBmsFilterIDs[0]) {
      bmsFlags |= FLAG_600;
      currentBmsAndElconData.BMS_sumOfCells = ((RxData[7] << 8) | RxData[6]) * 0.01;
      currentBmsAndElconData.BMS_avgVolt = currentBmsAndElconData.BMS_sumOfCells / 96;
      currentBmsAndElconData.BMS_packImbalance = ((RxData[3] << 8) | RxData[2]) * 0.0001;
    } else if (RxHeader.StdId == elconBmsFilterIDs[1]) {
      bmsFlags |= FLAG_621;
      currentBmsAndElconData.BMS_stateOfCharge = RxData[2];
    } else if (RxHeader.StdId == elconBmsFilterIDs[2]) {
      bmsFlags |= FLAG_622;
      currentBmsAndElconData.BMS_minTemp = RxData[5];
      currentBmsAndElconData.BMS_maxTemp = RxData[4];
      currentBmsAndElconData.BMS_minVolt = ((RxData[3] << 8) | RxData[2]) * 0.0001;
      currentBmsAndElconData.BMS_maxVolt = ((RxData[1] << 8) | RxData[0]) * 0.0001;
    }

    if (bmsFlags == (FLAG_600 | FLAG_621 | FLAG_622)) {
      PREVIOUS_TIME = CURRENT_TIME;
      bmsFlags = 0;
    }
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
  CAN_SettingsInit(&charging_msg, true, 8);

  // INIT BALANCING CAN STRUCT
  struct CANMessage balancing_msg;
  CAN_SettingsInit(&balancing_msg, false, 1);

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
  // while(true) {
  //   printf("INLET TEMP: %.2f\n", READ_THERM(therm_inlet, THERM_RESIST));
  //   HAL_Delay(1000);
  //   printf("OUTLET TEMP:%.2f\n", READ_THERM(therm_outlet, THERM_RESIST));
  //   HAL_Delay(1000);
  //   printBmsAndElconData(&currentBmsAndElconData);
  //   HAL_Delay(1000);
  // }
  FAN_SPD_CTRL(100); // TODO: make this based on temp
  uint16_t therm_inlet_value = adc_buffer[0];
  uint16_t therm_outlet_value = adc_buffer[1];
  therm_inlet = &therm_inlet_value;
  therm_outlet = &therm_outlet_value;
  GPIO_PinState IN_HVIL_SW_STATE;
  GPIO_PinState RTC_SW_STATE;
  GPIO_PinState IN_HVIL_ESTOP_Pin_State;
  GPIO_PinState IN_HVIL_CHAR_Pin_State;
  GPIO_PinState IN_HVIL_TERM_Pin_State;
  GPIO_PinState IN_HVIL_ACUM_Pin_State;
  GPIO_PinState IN_HVIL_FSW_Pin_State;
  char chargingInfoString[30];
   // TODO: better init for GUI
  // TEMP STUFF 1 END

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // TODO: CHECK ALL LEDS AND PERIPHERALS WORK
    //TODO: DOUBLE CHECK
    if ((!isCharging && !isBalancing && !isChargingSequence)) {
      CAN_Balance(&balancing_msg, false);
      SRE_Display_Test();
    }

	  ssd1306_Fill(Black);
	  ssd1306_UpdateScreen();

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
        isChargerSafe = true;
    else if (IN_HVIL_ESTOP_Pin_State == GPIO_PIN_RESET &&
        IN_HVIL_CHAR_Pin_State  == GPIO_PIN_RESET &&
        IN_HVIL_TERM_Pin_State  == GPIO_PIN_RESET &&
        IN_HVIL_ACUM_Pin_State  == GPIO_PIN_RESET)
        isChargerSafe = false;

    // TODO: add -> IN_HVIL_CHAR_Pin_State == GPIO_PIN_RESET
    
    if (!isChargerSafe)
    {
      CAN_Charge(&charging_msg, LIMIT_VOLTS, LIMIT_AMPS, false);
      ssd1306_Fill(Black);
	    ssd1306_UpdateScreen();
      ssd1306_SetCursor(5, 5);
      ssd1306_WriteString("HVIL ERROR", Font_6x8, White); // TODO: make this more clear
      ssd1306_UpdateScreen();
    } else if (isChargerSafe && IN_HVIL_FSW_Pin_State) {
      if(RTC_SW_STATE) {
        HAL_GPIO_WritePin(GPIOA, LED_HV_Pin, GPIO_PIN_SET);
        ssd1306_SetCursor(5, 5);
        CAN_Charge(&charging_msg, LIMIT_VOLTS, LIMIT_AMPS, false);
        isCharging = false;
        ssd1306_WriteString("PLS FLIP RTC", Font_6x8, White);
      } else if (!RTC_SW_STATE) {
        isChargingSequence = false;
        /*
            TODO: create a combo balance + charge function for safety
          - Ensure commanded output does not go above 4kW = I*V
          - When highest cell is >= 4.1v decrease current linearly until 4.25v then balance if imbalance >= 10mV then resume charging
        */
        if (LIMIT_AMPS * LIMIT_VOLTS <= MAX_ALLOWED_PWR)
        {
          if (currentBmsAndElconData.BMS_maxVolt >= UPPER_MAX_CELL_CV_THRESH)
          {
            // Stop charging and start balancing
            AMPS_AT_LOWER_MAX_CELL_CV_THRESH = currentBmsAndElconData.ELCON_outCurrent; // TODO: maybe change
            if (currentBmsAndElconData.BMS_packImbalance >= MIN_ALLOWED_IMBAL)
            {
              // Imbalance greater than 10mV => balance and stop charging
              CAN_Charge(&charging_msg, LIMIT_VOLTS, LIMIT_AMPS, false);
              isCharging = false;
              CAN_Balance(&balancing_msg, true);
              isBalancing = true;
            }
            else if (currentBmsAndElconData.BMS_packImbalance <= MIN_ALLOWED_IMBAL)
            {
              // Imbalance less than 10mV => charge at 0.5A and whatever the current pack voltage is
              CAN_Balance(&balancing_msg, false);
              isBalancing = false;
              // TODO: clean up and make part of main loop
              if ((currentBmsAndElconData.BMS_avgVolt * 96.0 * MAINT_AMPS) <= MAX_ALLOWED_PWR)
              {
                CAN_Charge(&charging_msg, currentBmsAndElconData.BMS_avgVolt * 96.0, MAINT_AMPS, true);
                isCharging = true;
              }
              else if ((currentBmsAndElconData.BMS_avgVolt * 96.0 * MAINT_AMPS) >= MAX_ALLOWED_PWR)
              {
                // TODO: add proper error state
              }
            }
          }
          else if (currentBmsAndElconData.BMS_maxVolt >= 4.1)
          {
            // Highest cell volt above 4.1v, now command current linearly
            CAN_Balance(&balancing_msg, false);
            isBalancing = false;
            float slope = (MAINT_AMPS - AMPS_AT_LOWER_MAX_CELL_CV_THRESH) / (UPPER_MAX_CELL_CV_THRESH - LOWER_MAX_CELL_CV_THRESH);
            CAN_Charge(&charging_msg, LIMIT_VOLTS, slope * LIMIT_VOLTS, true);
            isCharging = true;
          }
          else if (currentBmsAndElconData.BMS_maxVolt <= LOWER_MAX_CELL_CV_THRESH)
          {
            // Highest cell volt under 4.1v, charge normally and stop balancing
            CAN_Balance(&balancing_msg, false);
            isBalancing = false;
            CAN_Charge(&charging_msg, LIMIT_VOLTS, LIMIT_AMPS, true);
            isCharging = false;
          }
          } else if (LIMIT_AMPS * LIMIT_VOLTS <= MAX_ALLOWED_PWR) {
            // TODO: add proper error state
            // Over max power to pull => throw error and stop charging
          }
        // ssd1306_WriteString("Now Charging", Font_6x8, White);
        // ssd1306_SetCursor(5, 20);
        // ssd1306_WriteString(chargingInfoString, Font_6x8, White);
        if (currentChargingScreen == 1) {
          SRE_Display_Charging1();
        }
        else if (currentChargingScreen == 2) {
          SRE_Display_Charging2();
        }
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
