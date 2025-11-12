#ifndef CHARGER_H
#define CHARGER_H

#include "main.h"
#include "can_interface.h"
#include <stdbool.h>

#define UPPER_MAX_CELL_CV_THRESH 4.25 // Competition is 4.25
#define LOWER_MAX_CELL_CV_THRESH 4.1 // Competition is 4.1
#define MIN_ALLOWED_IMBAL 0.01
#define MAINT_AMPS 0.5
#define HYSTERESIS_VOLTS 0.01

typedef enum {
    CHARGING_MODE_CONSTANT_CURRENT,
    CHARGING_MODE_CURRENT_TAPER,
    CHARGING_MODE_MAINTENANCE,
    CHARGING_MODE_BALANCING,
} chargingMode;

typedef enum {
    CHARGER_STATE_IDLE,
    CHARGER_STATE_CHARGING,
    CHARGER_STATE_BALANCING,
} chargerState;

//TODO: Separate BMS and Elcon into different structs
typedef struct {
    float BMS_avgVolt;
    float BMS_sumOfCells;
    float BMS_minVolt;
    float BMS_maxVolt;
    float BMS_avgTemp;
    float BMS_minTemp;
    float BMS_maxTemp;
    float BMS_stateOfCharge; // TODO: not float
    float BMS_packImbalance;
    float BMS_balanceStatus;
    bool  BMS_fault[6];
    /*
      Bit 0: Cell High Temp Fault
      Bit 1: Cell Volt Imbalance Fault
      Bit 2: Cell Low Volt Fault
      Bit 3: Cell High Volt Fault
      Bit 4: Pack Low Volt Fault
      Bit 5: Pack High Volt Fault
    */
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
    
} bmsAndElconData;

extern volatile bmsAndElconData currentBmsAndElconData;
extern chargingMode currentChargingMode;
extern chargerState currentChargerState;
extern float LIMIT_VOLTS;
extern float LIMIT_AMPS;
extern uint16_t MAX_ALLOWED_PWR;

void Charger_updateChargingMode();
void Charger_handleCharging(CANMessage *charging_msg, CANMessage *balancing_msg);
bool Charger_isChargerSafe();
bool Charger_isHvilSwitchFlipped();
bool Charger_isReadyToChargeSwitchFlipped();
bool Charger_checkFaultStatus();
void Charger_printBmsAndElconData(const volatile bmsAndElconData *d);
void Charger_printPinStates();



#endif
