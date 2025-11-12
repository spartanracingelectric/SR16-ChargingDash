#ifndef CAN_INTERFACE_H
#define CAN_INTERFACE_H

#include "main.h"
#include <stdbool.h>

#define FLAG_600 (1 << 0)
#define FLAG_621 (1 << 1)
#define FLAG_622 (1 << 2)
#define FLAG_604 (1 << 3)

typedef struct {
    CAN_TxHeaderTypeDef TxHeader;
    uint32_t TxMailbox;
    uint8_t data[8];
} CANMessage;

extern CAN_HandleTypeDef hcan1;
extern CAN_RxHeaderTypeDef RxHeader;
extern uint8_t RxData[8];
extern uint32_t elconBmsFilterIDs[5];

HAL_StatusTypeDef CAN_Start();
HAL_StatusTypeDef CAN_Activate();
HAL_StatusTypeDef CAN_Send(CANMessage *canMsgPtr);
void CAN_SettingsInit(CANMessage *canMsgPtr, bool isExtended, uint16_t dlc_length);
void Set_CAN_Id(CANMessage *ptr, uint32_t id, bool isExtended);
void CAN_Balance(CANMessage *ptr, bool balancing_enabled);
void CAN_Charge(CANMessage *ptr, float chargingLimitsVoltsFloat, float chargingLimitsAmpsFloat, bool charge_enable);

#endif
