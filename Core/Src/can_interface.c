#include "can_interface.h"
#include "charger.h"
#include <stdio.h> 

static uint8_t bmsFlags = 0;

int bms_can_current_time = 0;
int bms_can_previous_time = 0;
int bms_can_debounce_ms = 1000;

uint32_t elconBmsFilterIDs[5] = {
  0x600, // BMS imbalance
  0x621, // BMS soc
  0x622, // BMS volt/temp high+low
  0x18FF50E5, // Elcon
  0x604, //BMS balance status
};

CAN_RxHeaderTypeDef RxHeader;
uint8_t RxData[8];

HAL_StatusTypeDef CAN_Start() {
	return HAL_CAN_Start(&hcan1);
}

HAL_StatusTypeDef CAN_Activate() {
	return HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

HAL_StatusTypeDef CAN_Send(CANMessage *canMsgPtr) {
	return HAL_CAN_AddTxMessage(&hcan1, &canMsgPtr->TxHeader, (uint8_t*) canMsgPtr->data, &canMsgPtr->TxMailbox);
}

void CAN_SettingsInit(CANMessage *canMsgPtr, bool isExtended, uint16_t dlc_length) {
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

  // ----- Filter 4: BMS Balance Status 0x604 -----
  CAN_FilterTypeDef filter4 = {0};
  filter4.FilterBank = 4;
  filter4.FilterMode = CAN_FILTERMODE_IDMASK;
  filter4.FilterScale = CAN_FILTERSCALE_32BIT;
  filter4.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  filter4.FilterActivation = ENABLE;
  filter4.FilterIdHigh     = (elconBmsFilterIDs[4] << 5) & 0xFFFF;
  filter4.FilterIdLow      = 0;
  filter4.FilterMaskIdHigh = 0xFFFF;
  filter4.FilterMaskIdLow  = 0xFFFF;

  // Apply filters
  HAL_CAN_ConfigFilter(&hcan1, &filter0);
  HAL_CAN_ConfigFilter(&hcan1, &filter1);
  HAL_CAN_ConfigFilter(&hcan1, &filter2);
  HAL_CAN_ConfigFilter(&hcan1, &filter3);
  HAL_CAN_ConfigFilter(&hcan1, &filter4);

  HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void Set_CAN_Id(CANMessage *ptr, uint32_t id, bool isExtended) {
  if (isExtended) {
	  ptr->TxHeader.ExtId = id;
  } else {
    ptr->TxHeader.StdId = id;
  }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK) {
    	Error_Handler();
  	}

  	bms_can_current_time = HAL_GetTick();
  	int time_difference = bms_can_current_time - bms_can_previous_time;

	if (RxHeader.IDE == CAN_ID_EXT && RxHeader.ExtId == elconBmsFilterIDs[3]) {
		currentBmsAndElconData.ELCON_outVolt = ((RxData[0] << 8) | RxData[1]) * 0.1;
		currentBmsAndElconData.ELCON_outCurrent = ((RxData[2] << 8) | RxData[3])* 0.1;
		currentBmsAndElconData.ELCON_fault[4] = RxData[4] & 0x10;
		currentBmsAndElconData.ELCON_fault[3] = RxData[4] & 0x08;
		currentBmsAndElconData.ELCON_fault[2] = RxData[4] & 0x04;
		currentBmsAndElconData.ELCON_fault[1] = RxData[4] & 0x02;
		currentBmsAndElconData.ELCON_fault[0] = RxData[4] & 0x01;
	}

	if (RxHeader.IDE == CAN_ID_STD && time_difference > bms_can_debounce_ms) {
		if (RxHeader.StdId == elconBmsFilterIDs[0]) {
			bmsFlags |= FLAG_600;
			currentBmsAndElconData.BMS_sumOfCells = ((RxData[7] << 8) | RxData[6]) * 0.01;
			currentBmsAndElconData.BMS_avgVolt = currentBmsAndElconData.BMS_sumOfCells / 96;
			currentBmsAndElconData.BMS_packImbalance = ((RxData[3] << 8) | RxData[2]) * 0.0001;

			uint8_t faultByte = RxData[1];
			currentBmsAndElconData.BMS_fault[0] = ((faultByte >> 2) & 0x1); // Cell High Temp Fault
			currentBmsAndElconData.BMS_fault[1] = ((faultByte >> 3) & 0x1); // Cell Volt Imbalance Fault
			currentBmsAndElconData.BMS_fault[2] = ((faultByte >> 4) & 0x1); // Cell Low Volt Fault
			currentBmsAndElconData.BMS_fault[3] = ((faultByte >> 5) & 0x1); // Cell High Volt Fault
			currentBmsAndElconData.BMS_fault[4] = ((faultByte >> 6) & 0x1); // Pack Low Volt Fault
			currentBmsAndElconData.BMS_fault[5] = ((faultByte >> 7) & 0x1); // Pack High Volt Fault
		} 
		else if (RxHeader.StdId == elconBmsFilterIDs[1]) {
			bmsFlags |= FLAG_621;
			currentBmsAndElconData.BMS_stateOfCharge = RxData[2];
   		} 
		else if (RxHeader.StdId == elconBmsFilterIDs[2]) {
			bmsFlags |= FLAG_622;
			currentBmsAndElconData.BMS_minTemp = RxData[5];
			currentBmsAndElconData.BMS_maxTemp = RxData[4];
			currentBmsAndElconData.BMS_minVolt = ((RxData[3] << 8) | RxData[2]) * 0.0001;
			currentBmsAndElconData.BMS_maxVolt = ((RxData[1] << 8) | RxData[0]) * 0.0001;
    	}
        else if (RxHeader.StdId == elconBmsFilterIDs[4]) {
			bmsFlags |= FLAG_604;
            currentBmsAndElconData.BMS_balanceStatus = (RxData[0] & 0x01) ? 1 : 0;
        }

		if (bmsFlags == (FLAG_600 | FLAG_621 | FLAG_622 | FLAG_604)) {
			bms_can_previous_time = bms_can_current_time;
			bmsFlags = 0;
		}
	}
}

void CAN_Balance(CANMessage *ptr, bool balancing_enabled) {
  uint32_t CAN_ID = 0x604;
  Set_CAN_Id(ptr, CAN_ID, false);
  ptr->data[0] = (balancing_enabled) ? 0x1 : 0x0;
  HAL_Delay(10);
  CAN_Send(ptr);
}

void CAN_Charge(CANMessage *ptr, float chargingLimitsVoltsFloat, float chargingLimitsAmpsFloat, bool charge_enable) {
  // Check for mailbox instead of delay
  uint32_t CAN_ID = 0x1806E5F4;
  Set_CAN_Id(ptr, CAN_ID, true);

  chargingLimitsVoltsFloat *= 10;
  chargingLimitsAmpsFloat *= 10;
  uint16_t chargingLimitsVolts = (uint16_t)chargingLimitsVoltsFloat;
  uint16_t chargingLimitsAmps = (uint16_t)chargingLimitsAmpsFloat;
  printf("CHARGING LIMIT VOLTS: %d, CHARGING LIMIT AMPS: %d", chargingLimitsVolts, chargingLimitsAmps);

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
