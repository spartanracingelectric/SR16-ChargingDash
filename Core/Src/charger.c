#include "charger.h"
#include "display.h"

float LIMIT_VOLTS = 0;
float LIMIT_AMPS = 0;
uint16_t MAX_ALLOWED_PWR = 10000;

volatile bmsAndElconData currentBmsAndElconData = {0};
charging_mode current_charging_mode = CHARGING_MODE_CONSTANT_CURRENT;
charger_state current_charger_state = CHARGER_STATE_IDLE;


void charger_update_charging_mode() {
    if (current_charger_state == CHARGER_STATE_BALANCING) {
        current_charging_mode = CHARGING_MODE_BALANCING;
        return;
    }

    if (currentBmsAndElconData.BMS_maxVolt >= UPPER_MAX_CELL_CV_THRESH) {
        if (currentBmsAndElconData.BMS_packImbalance >= MIN_ALLOWED_IMBAL) {
            current_charging_mode = CHARGING_MODE_BALANCING;
            return;
        }
        else {
            current_charging_mode = CHARGING_MODE_MAINTENANCE;
            return;
        }
    }

    if (currentBmsAndElconData.BMS_maxVolt >= LOWER_MAX_CELL_CV_THRESH 
        && currentBmsAndElconData.BMS_maxVolt < UPPER_MAX_CELL_CV_THRESH) {
        current_charging_mode = CHARGING_MODE_CURRENT_TAPER;
        return;
    }

    if (currentBmsAndElconData.BMS_maxVolt < LOWER_MAX_CELL_CV_THRESH) {
        current_charging_mode = CHARGING_MODE_CONSTANT_CURRENT;
        return;
    }
}

bool charger_check_charging_conditions() {
    if (current_charger_state == CHARGER_STATE_BALANCING) {
        return (charger_is_charger_safe() && charger_is_hvil_switch_flipped());
    }
    else {
        return (charger_is_charger_safe() && charger_is_hvil_switch_flipped() 
        && charger_is_ready_to_charge_switch_flipped())
    }
}

void charger_handle_charging(CANMessage *charging_msg, CANMessage *balancing_msg) {
    //If balancing or charging, always check charging conditions
    if (current_charger_state != CHARGER_STATE_IDLE && !charger_check_charging_conditions()) {
        current_charger_state = CHARGER_STATE_IDLE;
        CAN_Charge(&charging_msg, LIMIT_VOLTS, LIMIT_AMPS, false);
        CAN_Balance(&balancing_msg, false);
        HAL_GPIO_WritePin(GPIOA, LED_BAL_Pin, GPIO_PIN_RESET);
        next_display_state = DISPLAY_STATE_NAVIGATION;
        return;
    }

    if (current_charger_state == CHARGER_STATE_IDLE) {
        CAN_Charge(&charging_msg, LIMIT_VOLTS, LIMIT_AMPS, false);
        CAN_Balance(&balancing_msg, false);
        HAL_GPIO_WritePin(GPIOA, LED_BAL_Pin, GPIO_PIN_RESET);
        return;
    }

    if (current_charger_state == CHARGER_STATE_CHARGING) {
        charger_update_charging_mode();
        if (current_charging_mode == CHARGING_MODE_BALANCING) {
            CAN_Charge(charging_msg, LIMIT_VOLTS, LIMIT_AMPS, false);
            CAN_Balance(balancing_msg, true);
            HAL_GPIO_WritePin(GPIOA, LED_BAL_Pin, GPIO_PIN_SET);
        }
        else if (current_charging_mode == CHARGING_MODE_MAINTENANCE) {
            CAN_Charge(charging_msg, currentBmsAndElconData.BMS_sumOfCells, MAINT_AMPS, false);
            CAN_Balance(balancing_msg, false);
            HAL_GPIO_WritePin(GPIOA, LED_BAL_Pin, GPIO_PIN_RESET);

        }
        else if (current_charging_mode == CHARGING_MODE_CURRENT_TAPER) {
            float current = LIMIT_AMPS + ((MAINT_AMPS - LIMIT_AMPS) / (UPPER_MAX_CELL_CV_THRESH - LOWER_MAX_CELL_CV_THRESH) * (currentBmsAndElconData.BMS_maxVolt - LOWER_MAX_CELL_CV_THRESH));
            CAN_Charge(charging_msg, LIMIT_VOLTS, current, true);
            CAN_Balance(balancing_msg, false);
            HAL_GPIO_WritePin(GPIOA, LED_BAL_Pin, GPIO_PIN_RESET);
        }
        else if (current_charging_mode == CHARGING_MODE_CONSTANT_CURRENT) {
            CAN_Charge(charging_msg, LIMIT_VOLTS, LIMIT_AMPS, true);
            CAN_Balance(balancing_msg, false);
            HAL_GPIO_WritePin(GPIOA, LED_BAL_Pin, GPIO_PIN_RESET);
        }
        else {
            CAN_Charge(charging_msg, LIMIT_VOLTS, LIMIT_AMPS, false);
            CAN_Balance(balancing_msg, false);
            HAL_GPIO_WritePin(GPIOA, LED_BAL_Pin, GPIO_PIN_RESET);
        }
    }
    
    if (current_charger_state == CHARGER_STATE_BALANCING)  {
        charger_update_charging_mode();
        if (current_charging_mode == CHARGING_MODE_BALANCING) {
            CAN_Charge(charging_msg, LIMIT_VOLTS, LIMIT_AMPS, false);
            CAN_Balance(balancing_msg, true);
            HAL_GPIO_WritePin(GPIOA, LED_BAL_Pin, GPIO_PIN_SET);
        }
    }
}

bool charger_is_charger_safe() {
    GPIO_PinState IN_HVIL_CHAR_Pin_State;
    GPIO_PinState IN_HVIL_ESTOP_Pin_State = HAL_GPIO_ReadPin(IN_HVIL_ESTOP_GPIO_Port, IN_HVIL_ESTOP_Pin);
    GPIO_PinState IN_HVIL_TERM_Pin_State = HAL_GPIO_ReadPin(IN_HVIL_TERM_GPIO_Port, IN_HVIL_TERM_Pin);
    GPIO_PinState IN_HVIL_ACUM_Pin_State = HAL_GPIO_ReadPin(IN_HVIL_ACUM_GPIO_Port, IN_HVIL_ACUM_Pin);

    if (IN_HVIL_ESTOP_Pin_State == GPIO_PIN_SET ||
        IN_HVIL_CHAR_Pin_State  == GPIO_PIN_SET ||
        IN_HVIL_TERM_Pin_State  == GPIO_PIN_SET ||
        IN_HVIL_ACUM_Pin_State  == GPIO_PIN_SET) {
            return true;
    } 
    else if (IN_HVIL_ESTOP_Pin_State == GPIO_PIN_RESET &&
        IN_HVIL_CHAR_Pin_State  == GPIO_PIN_RESET &&
        IN_HVIL_TERM_Pin_State  == GPIO_PIN_RESET &&
        IN_HVIL_ACUM_Pin_State  == GPIO_PIN_RESET) {
            return false;
    }
    return false;
}

bool charger_is_hvil_switch_flipped() {
    GPIO_PinState IN_HVIL_FSW_STATE = HAL_GPIO_ReadPin(IN_HVIL_FSW_GPIO_Port, IN_HVIL_FSW_Pin);
    if (IN_HVIL_FSW_STATE) {
        return true;
    }
    else {
        return false;
    }
}

bool charger_is_ready_to_charge_switch_flipped() {
    GPIO_PinState RTC_SW_STATE = HAL_GPIO_ReadPin(IN_RTC_SW_GPIO_Port, IN_RTC_SW_Pin);
    if (RTC_SW_STATE) {
        return false;
    }
    else {
        return true;
    }
}

void charger_print_bms_and_elcon_data(const volatile bmsAndElconData *d) {
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
