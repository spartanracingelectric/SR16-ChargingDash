// Written by Ayman Alamayri in Dec 2024
#include "display.h"
#include "charger.h"
#include <stdio.h>

profile allProfiles[] = {
	{"P1", 3, 355},
	{"P2", 4, 355},
	{"P3", 20, 355},
	{"P4", 3, 385},
	{"P5", 15, 385},
	{"P6", 20, 385},
	{"P7", 10, 401},
	{"P8", 20, 400},
	{"P9", 10, 403},	
};

int numberOfProfiles = 9;

int selectedOption = 0;
bool selectPressed = false;
bool backPressed = false;

displayState currentDisplayState = DISPLAY_STATE_NAVIGATION;
displayState nextDisplayState = DISPLAY_STATE_NAVIGATION;


extern char codeBranch[10];
extern char codeVersion[5];

uint32_t buttonInterruptCurrentTime = 0;
uint32_t buttonInterruptPreviousTime = 0;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == BTN_UP_Pin || GPIO_Pin == BTN_DWN_Pin || GPIO_Pin == BTN_SEL_Pin || GPIO_Pin == BTN_BCK_Pin) {
  	    // TODO: fix debouncing
        buttonInterruptCurrentTime = HAL_GetTick();
        int debounceTimeThreshold = 200;
        int timeDifference = buttonInterruptCurrentTime - buttonInterruptPreviousTime;
        if (timeDifference > debounceTimeThreshold) {
            if (GPIO_Pin == BTN_UP_Pin) {
                selectedOption--;
            }   
            else if (GPIO_Pin == BTN_DWN_Pin) {
                selectedOption++;
            } 
            else if (GPIO_Pin == BTN_SEL_Pin) {
                selectPressed = true;
            } 
            else if (GPIO_Pin == BTN_BCK_Pin) {
                backPressed = true;
            }
            buttonInterruptPreviousTime= buttonInterruptCurrentTime;
        }
    }
}

void DISP_KanoaSplash() {
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	ssd1306_SetCursor(64, 15);
	ssd1306_WriteString("Elcon Control", Font_6x8, White);
	ssd1306_SetCursor(64, 25);
	ssd1306_WriteString("By Ayman A., et al", Font_6x8, White);
	ssd1306_SetCursor(64, 35);
	ssd1306_WriteString(codeVersion, Font_6x8, White);
	ssd1306_SetCursor(64, 45);
	ssd1306_WriteString(codeBranch, Font_6x8, White);
	ssd1306_UpdateScreen();
}

void Display_init() {
	ssd1306_Init();
}

displayState Display_updateState() {
	if (currentDisplayState != nextDisplayState) {
		selectedOption = 0;
		selectPressed = false;
		currentDisplayState = nextDisplayState;
	}

	switch(currentDisplayState) {
		case DISPLAY_STATE_NAVIGATION:
			nextDisplayState = Display_displayNavigation(); 
			break;
		case DISPLAY_STATE_HOME:
			nextDisplayState = Display_displayHome();
			break;
		case DISPLAY_STATE_CHARGING_PROFILES:
			nextDisplayState = Display_displayChargingProfiles();
			break;
		case DISPLAY_STATE_CHARGING_INITIALIZATION:
			nextDisplayState = Display_displayChargingInitialization();
			break;
		case DISPLAY_STATE_START_BALANCING:
			nextDisplayState = Display_displayStartBalancing();
			break;
		case DISPLAY_STATE_BALANCING_INITIALIZATION:
			nextDisplayState = Display_displayBalancingInitialization();
			break;
		case DISPLAY_STATE_BATTERY_STATS_ONE:
			nextDisplayState = Display_displayBatteryStatsOne();
			break;
		case DISPLAY_STATE_BATTERY_STATS_TWO:
			nextDisplayState = Display_displayBatteryStatsTwo();
			break;
		case DISPLAY_STATE_CHARGER_STATS:
			nextDisplayState = Display_displayChargerStats();
			break;
		case DISPLAY_STATE_IN_CHARGING_STATS_ONE:
			nextDisplayState = Display_displayInChargingStatsOne();
			break;
		case DISPLAY_STATE_IN_CHARGING_STATS_TWO:
			nextDisplayState = Display_displayInChargingStatsTwo();
			break;
		case DISPLAY_STATE_ERRORS:
			nextDisplayState = Display_displayErrors();
			break;
	}
	return nextDisplayState;
}

void Display_clear() {
	ssd1306_FillRectangle(0, 0, 127, 63, Black);
}

void Display_wrapSelectedOption(int numberOfOptions) {
	if (selectedOption > numberOfOptions - 1 ){
		selectedOption = 0;
	}
	if (selectedOption < 0) {
		selectedOption = numberOfOptions - 1;
	}
}

void Display_checkSelectedOptionBounds(int numberOfOptions) {
	if (selectedOption > numberOfOptions - 1) {
		selectedOption = 0;
	}
	if (selectedOption < 0) {
		selectedOption = numberOfOptions;
	}
}

displayState Display_displayNavigation() {
	static char* options[] = {"Home", "Charging", "Balancing", "Battery", "Charger Stats", "Errors", "Restart"};
	int numberOfOptions = 7;
	int y1 = 15;
	int y2 = 13;
	int y3 = 24;
	int currentView = selectedOption/4;
	int startIndex = currentView*4;

	Display_clear();
	Display_wrapSelectedOption(numberOfOptions);

	Display_drawTitleBar("Navigation");

	for (int i = startIndex; i < startIndex + 4 && i < numberOfOptions; i++) {
		ssd1306_SetCursor(3, y1);
		if (selectedOption == i) {
			ssd1306_FillRectangle(1,y2, 122, y3, White);
			ssd1306_WriteString(options[i], Font_6x8, Black);
		}
		else {
			ssd1306_DrawRectangle(1,y2, 122, y3, White);
			ssd1306_WriteString(options[i], Font_6x8, White);
		}

		y1 = y1 + 13;
		y2 = y2 + 13;
		y3 = y2 + 10;
	}

	int numberOfViews = (numberOfOptions + 3) / 4; //3 options per view, rounds up to ensure there is enough views
	Display_drawLongScrollBar(currentView, numberOfViews);

	Display_updateScreen();
	
	if (selectPressed) {
		Display_checkSelectedOptionBounds(numberOfOptions);
		switch(selectedOption) {
			case 0: return DISPLAY_STATE_HOME;
			case 1: return DISPLAY_STATE_CHARGING_PROFILES;
			case 2: return DISPLAY_STATE_START_BALANCING;
			case 3: return DISPLAY_STATE_BATTERY_STATS_ONE;
			case 4: return DISPLAY_STATE_CHARGER_STATS;
			case 5: return DISPLAY_STATE_ERRORS;
			case 6: NVIC_SystemReset();
		}
	}
	return DISPLAY_STATE_NAVIGATION;
}

void Display_forceI2CReset() {
    printf("Forcing I2C reset...\n");
    
	//Abort any ongoing operations
    HAL_I2C_Master_Abort_IT(&hi2c2, SSD1306_I2C_ADDR);
    HAL_Delay(10);
    
    //Force reset I2C peripheral
    __HAL_RCC_I2C2_FORCE_RESET();
    HAL_Delay(10);
    __HAL_RCC_I2C2_RELEASE_RESET();
    HAL_Delay(10);
    
    //Reinitialize I2C
    MX_I2C2_Init();
    HAL_Delay(10);
    
    //Reinitialize display
    ssd1306_Init();
    
    printf("I2C reset complete\n");
}

void Display_updateScreen() {
	ssd1306_UpdateScreen();
	if (ssd1306_Custom_GetLastStatus() != HAL_OK) {
		Display_forceI2CReset();
	}
}

//TODO: FINISH
displayState Display_displayHome() {
	char stateOfCharge[50];
	char balancingStatus[50];

	//TODO READ CHARGER TEMP
	char chargerTemp[] = "Charger Tmp: N/A";
	int numberOfOptions = 1;

	Display_clear();
	Display_wrapSelectedOption(numberOfOptions);


	Display_drawTitleBar("Home");

	ssd1306_SetCursor(1, 13);
	ssd1306_WriteString(stateOfCharge, Font_6x8, White);

	ssd1306_SetCursor(1, 23);
	ssd1306_WriteString(chargerTemp, Font_6x8, White);

	ssd1306_SetCursor(1, 33);
	ssd1306_WriteString(balancingStatus, Font_6x8, White);

	char *navBarOptions[] = {"Nav"};
	int firstNavBarOptionIndex= 0;
	Display_drawNavBar(navBarOptions, numberOfOptions, firstNavBarOptionIndex);

	Display_updateScreen();

	if (selectPressed) {
		Display_checkSelectedOptionBounds(numberOfOptions);
		switch(selectedOption) {
			case 0: return DISPLAY_STATE_NAVIGATION;
		}
	}
	return DISPLAY_STATE_HOME;
}

void Display_drawNavBar(char *options[], int numberOfNavBarOptions, int firstNavBarOptionIndex) {

	int navBarOptionIndex = firstNavBarOptionIndex;

	int x1 = 1;
	int x2 = 1;

	for (int i = 0; i < numberOfNavBarOptions; i++) {
		x2 = x1 + (strlen(options[i]) * 6) + 2;
		if (selectedOption == navBarOptionIndex) {
			ssd1306_FillRectangle(x1, 52, x2, 62, White);
			ssd1306_SetCursor(x1 + 2, 54);
			ssd1306_WriteString(options[i], Font_6x8, Black);
		}
		else {
			ssd1306_DrawRectangle(x1, 52, x2, 62, White);
			ssd1306_SetCursor(x1 + 2, 54);
			ssd1306_WriteString(options[i], Font_6x8, White);
		}

		navBarOptionIndex++;
		x1 = x2 + 2;
	}
}

//TODO: Title should only display Balancing if charger state is balancing, or it should show BAL symbol if auto balancing in charger state 
displayState Display_displayInChargingStatsTwo() {
	if (currentChargerState != CHARGER_STATE_CHARGING && 
		currentChargerState != CHARGER_STATE_BALANCING) {
		return DISPLAY_STATE_NAVIGATION;
	}

	int numberOfOptions = 1;

	char sumOfCells[50];
	char stateOfCharge[50];
	char averageStats[50];
	char chargingInfo[50];

	sprintf(averageStats, "Avg V:%.3fV", currentBmsAndElconData.BMS_avgVolt);
	sprintf(chargingInfo, "%.2f V @ %.2f A", LIMIT_VOLTS, LIMIT_AMPS);
	sprintf(stateOfCharge, "SOC:%.2f%%", currentBmsAndElconData.BMS_stateOfCharge);
	sprintf(sumOfCells, "Pack Volt: %.2fV", currentBmsAndElconData.BMS_sumOfCells);

	Display_clear();
	Display_wrapSelectedOption(numberOfOptions);

	bool isBalancing = (currentChargerState == CHARGER_STATE_BALANCING);

	Display_drawTitleBar(isBalancing ? "Balancing 2" : "Charging 2");

	ssd1306_SetCursor(1, 13);
	ssd1306_WriteString(sumOfCells, Font_6x8, White);

	//Writes SOC Stats
	ssd1306_SetCursor(1, 23);
	ssd1306_WriteString(stateOfCharge, Font_6x8, White);

	ssd1306_SetCursor(1, 33);
	ssd1306_WriteString(averageStats, Font_6x8, White);

	ssd1306_SetCursor(1, 43);
	ssd1306_WriteString(chargingInfo, Font_6x8, White);


	char *navBarOptions[1] = { isBalancing ? "Balancing 1" : "Charging 1" };
	int firstNavBarOptonIndex = 0;
	Display_drawNavBar(navBarOptions, numberOfOptions, firstNavBarOptonIndex);
    Display_updateScreen();

	if (selectPressed) {
		Display_checkSelectedOptionBounds(numberOfOptions);
		switch(selectedOption) {
			case 0: return DISPLAY_STATE_IN_CHARGING_STATS_ONE;
		}
	}
	return DISPLAY_STATE_IN_CHARGING_STATS_TWO;
}

displayState Display_displayChargingProfiles() {

	//TODO: move calculation outside of this, don't want to run this everytime
	profile profiles[numberOfProfiles];
	
	int profileIndex = 0;
	for (int i = 0; i < numberOfProfiles; i++) {
		if (allProfiles[i].voltage * allProfiles[i].current <= (MAX_ALLOWED_PWR * 97 / 100) && allProfiles[i].voltage > currentBmsAndElconData.BMS_sumOfCells) {
			profiles[profileIndex] = allProfiles[i];
			profileIndex++;
		}
	}

	int currentNumberOfProfiles = profileIndex;

  	int navStartIndex = currentNumberOfProfiles;
  	int navLastIndex = currentNumberOfProfiles;

	Display_clear();
	Display_drawTitleBar("Charging");

	int currentScreen = selectedOption/3;


	//if the nav bar is selected, ensures that the currentScreen is the last screen of profiles
	if (selectedOption > currentNumberOfProfiles-1) {
		currentScreen = (currentNumberOfProfiles-1)/3;
	}
	int startIndex = currentScreen*3;
	//ensures that the correct number of profiles are showed on the last screen
	if (selectedOption > currentNumberOfProfiles - 1) {
		startIndex = (currentNumberOfProfiles - 1)/3*3;
	}
	//going up from first profile will go to Start button
	if (selectedOption < 0) {
		startIndex = 0;
		selectedOption = navLastIndex;
	}
	//going down from start button will reset back to first profile being selected
	if (selectedOption > navLastIndex) {
		startIndex = 0;
		selectedOption = 0;
	}

	//initial y-positions used for calculating profile display boxes
	int y1 = 15;
	int y2 = 13;
	int y3 = 24;

	//displays up to three profiles per screen
	for (int i = startIndex; i < startIndex + 3 && i < currentNumberOfProfiles; i++) {
		char profileString[50];
		sprintf(profileString, "%s: %dA %dV", profiles[i].name, profiles[i].current, profiles[i].voltage);
		ssd1306_SetCursor(3, y1);

		if (selectedOption == i) {
			ssd1306_FillRectangle(1,y2, 122, y3, White);
			ssd1306_WriteString(profileString, Font_6x8, Black);
		}
		else {
			ssd1306_DrawRectangle(1,y2, 122, y3, White);
			ssd1306_WriteString(profileString, Font_6x8, White);
		}

		y1 = y1 + 13;
		y2 = y2 + 13;
		y3 = y3 + 13;
	}

	//numOfProiles + 2 ensures it will always round up
	int numberOfScreens = (numberOfProfiles + 2)/3;

	Display_drawShortScrollBar(currentScreen, numberOfScreens);

	char *navBarOptions[] = {"Nav"};
	int numberOfNavBarOptions = 1;
	Display_drawNavBar(navBarOptions, numberOfNavBarOptions, navStartIndex);

	Display_updateScreen();

	if (selectPressed) {
	    // Make sure the selected option is within the valid range of profiles
	    if (selectedOption >= 0 && selectedOption < currentNumberOfProfiles) {
	        profile selectedProfile = profiles[selectedOption];
	        // Set charging limits based on the selected profile
			LIMIT_VOLTS = selectedProfile.voltage;
			LIMIT_AMPS = selectedProfile.current;
			return DISPLAY_STATE_CHARGING_INITIALIZATION;
	    }
	    else if (selectedOption == currentNumberOfProfiles) {
			return DISPLAY_STATE_NAVIGATION;
	    }
	}
	return DISPLAY_STATE_CHARGING_PROFILES;
}

displayState Display_displayChargingInitialization() {
	Display_clear();

	if (!Charger_isChargerSafe()) {
    	ssd1306_SetCursor(5, 5);
		ssd1306_WriteString("HVIL ERROR", Font_6x8, White);
		Display_updateScreen();
	}
	else if (!Charger_isHvilSwitchFlipped()) {
		ssd1306_SetCursor(5, 5);
		ssd1306_WriteString("PLEASE FLIP HV", Font_6x8, White);
		Display_updateScreen();
	}
	else if (!Charger_isReadyToChargeSwitchFlipped()) {
		ssd1306_SetCursor(5, 5);
		ssd1306_WriteString("PLEASE FLIP RTC", Font_6x8, White);
		Display_updateScreen();
	}
	else {
		currentChargerState = CHARGER_STATE_CHARGING;
		return DISPLAY_STATE_IN_CHARGING_STATS_ONE;
	}
	return DISPLAY_STATE_CHARGING_INITIALIZATION;
}

void Display_drawShortScrollBar(int currentView, int numberOfViews) {
	//current_view is zero-indexed
	if (numberOfViews > 1) {
		int scrollContainerHeight = 35;
		int scrollBarLength = scrollContainerHeight/numberOfViews;
		int scrollBarStart = 14 + (currentView * scrollBarLength);

		ssd1306_DrawRectangle(124, 13, 126, 47, White);
		ssd1306_Line(125, scrollBarStart, 125, scrollBarStart + scrollBarLength, White);
	}
}

void Display_drawLongScrollBar(int currentView, int numberOfViews) {
	//currentScreen is zero-indexed
	if (numberOfViews > 1) {
		int scrollContainerHeight = 49;
		int scrollBarLength = scrollContainerHeight/numberOfViews;
		int scrollBarStart = 14+ (currentView * numberOfViews);

		ssd1306_DrawRectangle(124, 13, 126, 62, White);
		ssd1306_Line(125, scrollBarStart, 125, scrollBarStart + scrollBarLength, White);
	}
}
//TODO: This screen is pretty useless
displayState Display_displayChargerStats() {
	char inletTempString[50];
	char outletTempString[50];
	int numberOfOptions = 1;

	Display_clear();
	Display_wrapSelectedOption(numberOfOptions);

	sprintf(inletTempString, "Inlet Tmp: N/A");
	sprintf(outletTempString, "Outlet Tmp: N/A");

	Display_drawTitleBar("Charger Stats");

	ssd1306_SetCursor(1, 13);
	ssd1306_WriteString(inletTempString, Font_6x8, White);

	ssd1306_SetCursor(1, 23);
	ssd1306_WriteString(outletTempString, Font_6x8, White);

	char *navBarOptions[] = {"Nav"};
	int numberOfNavBarOptions = 1;
	int firstNavBarOptionIndex = 0;
	Display_drawNavBar(navBarOptions, numberOfNavBarOptions, firstNavBarOptionIndex);

	Display_updateScreen();

	if (selectPressed) {
		Display_checkSelectedOptionBounds(numberOfOptions);
		switch (selectedOption) {
			case 0: return DISPLAY_STATE_NAVIGATION;
		}
	}
	return DISPLAY_STATE_CHARGER_STATS;
}

displayState Display_displayBatteryStatsOne(){
	char temperatureStats[50];
	char voltageStats[50];
	char averageStats[50];
	int numberOfOptions = 2;

	sprintf(temperatureStats, "Tmp H/L:%.2f/%.2fC",
		currentBmsAndElconData.BMS_maxTemp,
		currentBmsAndElconData.BMS_minTemp);
	sprintf(voltageStats, "Vlt H/L:%.3f/%.3fV",
		currentBmsAndElconData.BMS_maxVolt,
		currentBmsAndElconData.BMS_minVolt);
	sprintf(averageStats, "Avg V:%.3fV",
		currentBmsAndElconData.BMS_avgVolt);

	Display_clear();
	Display_wrapSelectedOption(numberOfOptions);

	Display_drawTitleBar("Battery 1");

	ssd1306_SetCursor(1, 13);
	ssd1306_WriteString(temperatureStats, Font_6x8, White);

	ssd1306_SetCursor(1, 23);
	ssd1306_WriteString(voltageStats, Font_6x8, White);

	ssd1306_SetCursor(1, 33);
	ssd1306_WriteString(averageStats, Font_6x8, White);

	char *navBarOptions[] = {"Nav", "Battery 2"};
	int firstNavBarOptionIndex= 0;

	Display_drawNavBar(navBarOptions, numberOfOptions, firstNavBarOptionIndex);

	Display_updateScreen();
	

	if (selectPressed) {
		Display_checkSelectedOptionBounds(numberOfOptions);
		switch(selectedOption) {
			case 0: return DISPLAY_STATE_NAVIGATION;
			case 1: return DISPLAY_STATE_BATTERY_STATS_TWO;
		}
	}
	return DISPLAY_STATE_BATTERY_STATS_ONE;
}


displayState Display_displayBatteryStatsTwo(){

	char stateOfCharge[50];
	char packVolt[50];
	char packImbalance[50];
	int numberOfOptions = 2;

	sprintf(stateOfCharge, "SOC:%.2f%%",
			currentBmsAndElconData.BMS_stateOfCharge);
			
	sprintf(packImbalance, "Imbalance:%.2fV",
			currentBmsAndElconData.BMS_packImbalance);
	
	sprintf(packVolt, "Pack Volt: %.2fV",
			currentBmsAndElconData.BMS_sumOfCells);


	Display_clear();
	Display_wrapSelectedOption(numberOfOptions);

	Display_drawTitleBar("Battery 2");

	ssd1306_SetCursor(1, 13);
	ssd1306_WriteString(stateOfCharge, Font_6x8, White);

	ssd1306_SetCursor(1, 23);
	ssd1306_WriteString(packImbalance, Font_6x8, White);

	ssd1306_SetCursor(1, 33);
	ssd1306_WriteString(packVolt, Font_6x8, White);

	
	char *navBarOptions[] = {"Nav", "Battery 1"};
	int firstNavBarOptionIndex = 0;
	Display_drawNavBar(navBarOptions, numberOfOptions, firstNavBarOptionIndex);

	Display_updateScreen();
	

	if (selectPressed) {
		Display_wrapSelectedOption(numberOfOptions);
		switch(selectedOption) {
			case 0: return DISPLAY_STATE_NAVIGATION;
			case 1: return DISPLAY_STATE_BATTERY_STATS_ONE;
		}
	}
	return DISPLAY_STATE_BATTERY_STATS_TWO;
}

//TODO: Fix logic to display if its already balancing
displayState Display_displayStartBalancing() {
	int numberOfOptions = 2;

	Display_clear();
	Display_wrapSelectedOption(numberOfOptions);

	Display_drawTitleBar("Start Balancing");

	ssd1306_SetCursor(1, 13);
	ssd1306_WriteString("Balancing is off", Font_6x8, White);

	char *navBarOptions[] = {"Nav", "Start Bal"};
	int navBarStartIndex = 0;
	Display_drawNavBar(navBarOptions, numberOfOptions, navBarStartIndex);


	Display_updateScreen();
	
	if (selectPressed) {
		Display_checkSelectedOptionBounds(numberOfOptions);
		switch (selectedOption) {
			case 0: return DISPLAY_STATE_NAVIGATION;
			case 1: {
				return DISPLAY_STATE_BALANCING_INITIALIZATION;
			}
		}
	}
	return DISPLAY_STATE_START_BALANCING;
}

displayState Display_displayBalancingInitialization() {
	Display_clear();
	if (!Charger_isChargerSafe()) {
    	ssd1306_SetCursor(5, 5);
		ssd1306_WriteString("HVIL ERROR", Font_6x8, White);
		Display_updateScreen();
	}
	else if (!Charger_isReadyToChargeSwitchFlipped()) {
		ssd1306_SetCursor(5, 5);
		ssd1306_WriteString("PLEASE FLIP RTC", Font_6x8, White);
		Display_updateScreen();
	}
	else {
		currentChargerState = CHARGER_STATE_BALANCING;
		return DISPLAY_STATE_IN_CHARGING_STATS_ONE;
	}
	return DISPLAY_STATE_BALANCING_INITIALIZATION;
}

//TODO: Finish
void Display_drawTitleBar(char title[]) {
	bool isFault = false;
	for (int i = 0; i < 5; i++) {
		if (currentBmsAndElconData.ELCON_fault[i] == 1) {
			isFault = true;
			break;
		}
	}
    for (int i = 0; i < 6; i++) {
        if (currentBmsAndElconData.BMS_fault[i] == 1) {
            isFault = true;
            break;
        }
    }

	ssd1306_SetCursor(1, 1);
	ssd1306_WriteString(title, Font_6x8, White);
	ssd1306_Line(0, 10, 127, 10, White);

	//Flashing status symbols 
	// ssd1306_FillRectangle(70, 0, 127, 9, Black);
	// ssd1306_UpdateScreen();
	bool isCharging = currentChargerState != CHARGER_STATE_IDLE && (currentChargingMode == CHARGING_MODE_CONSTANT_CURRENT || 
						currentChargingMode == CHARGING_MODE_CURRENT_TAPER || 
						currentChargingMode == CHARGING_MODE_MAINTENANCE);
	bool isBalancing = currentBmsAndElconData.BMS_balanceStatus;
	if (isFault) {
		Display_drawErrorSymbol(119,1);
		if (isCharging) {
			Display_drawInChargingSymbol(92, 3);
		}
		else if (isBalancing) {
			ssd1306_FillRectangle(91, 0, 109, 8, White);
			ssd1306_SetCursor(92, 1);
			ssd1306_WriteString("BAL", Font_6x8, Black);
		}
	}
	else if (isCharging) {
		Display_drawInChargingSymbol(109, 3);
	}
	else if (isBalancing) {
		ssd1306_FillRectangle(108, 0, 126, 8, White);
		ssd1306_SetCursor(109, 1);
		ssd1306_WriteString("BAL", Font_6x8, Black);
	}
}

void Display_drawInChargingSymbol(int x, int y) {
	//point of origin (x,y) is the top left of battery
	ssd1306_Line(x, y, x+4, y, White);
	ssd1306_Line(x, y, x, y+4, White);
	ssd1306_Line(x, y+4, x+4, y+4, White);

	ssd1306_Line(x+12, y, x+16, y, White);
	ssd1306_Line(x+16, y, x+16, y+4, White);
	ssd1306_Line(x+12, y+4, x+16, y+4, White);
	ssd1306_Line(x+17, y+1, x+17, y+3, White);

	ssd1306_Line(x+6, y+2, x+10, y+2, White);
	ssd1306_Line(x+6, y+2, x+9, y-1, White);
	ssd1306_Line(x+10, y+2, x+7, y+5, White);
}

void Display_drawErrorSymbol(int x, int y) {
	//point of origin (x,y) is the top of the triangle
	ssd1306_Line(x, y, x+7, y+7, White);
	ssd1306_Line(x, y, x-7, y+7, White);
	ssd1306_Line(x-7, y+7, x+7, y+7, White);

	ssd1306_Line(x, y+2, x, y+4, White);
	ssd1306_Line(x, y+6, x, y+6, White);
}

//TODO: Title should only display Balancing if charger state is balancing, or it should show BAL symbol if auto balancing in charger state 
displayState Display_displayInChargingStatsOne() {
	if (currentChargerState != CHARGER_STATE_CHARGING && 
		currentChargerState != CHARGER_STATE_BALANCING) {
		return DISPLAY_STATE_NAVIGATION;
	}
	int numberOfOptions = 1;
	char temperatureStats[50];
	char voltageStats[50];
	char imbalance[30];
	char outputStats[50];

	sprintf(temperatureStats, "Tmp H/L:%.2f/%.2fC",
			currentBmsAndElconData.BMS_maxTemp,
			currentBmsAndElconData.BMS_minTemp);
	sprintf(voltageStats, "Vlt H/L:%.3f/%.3fV",
			currentBmsAndElconData.BMS_maxVolt,
			currentBmsAndElconData.BMS_minVolt);
	sprintf(imbalance, "Imbal:%.3fV", currentBmsAndElconData.BMS_packImbalance);
	sprintf(outputStats, "Out V/C:%.2fV/%.2fA", currentBmsAndElconData.ELCON_outVolt, currentBmsAndElconData.ELCON_outCurrent);

	Display_clear();
	Display_wrapSelectedOption(numberOfOptions);
	
	bool isBalancing = (currentChargerState == CHARGER_STATE_BALANCING);
	//Writes title
	Display_drawTitleBar(isBalancing ? "Balancing 1" : "Charging 1");

	//Writes temp
	ssd1306_SetCursor(1, 13);
	ssd1306_WriteString(temperatureStats, Font_6x8, White);

	//Writes voltage
	ssd1306_SetCursor(1, 23);	
	ssd1306_WriteString(voltageStats, Font_6x8, White);

	//Writes imbalance
	ssd1306_SetCursor(1, 33);
	ssd1306_WriteString(imbalance, Font_6x8, White);

	//Writes output info
	if (!isBalancing) {
		ssd1306_SetCursor(1, 43);
		ssd1306_WriteString(outputStats, Font_6x8, White);
	}
	
	char *navBarOptions[1] = { isBalancing ? "Balancing 2" : "Charging 2" };

	int firstNavBarOptionIndex = 0;
	Display_drawNavBar(navBarOptions, numberOfOptions, firstNavBarOptionIndex);
    Display_updateScreen();

	if (selectPressed) {
		Display_checkSelectedOptionBounds(numberOfOptions);
		switch (selectedOption) {
			case 0: return DISPLAY_STATE_IN_CHARGING_STATS_TWO;
		}
	}
	return DISPLAY_STATE_IN_CHARGING_STATS_ONE;
}

displayState Display_displayErrors() {
	
	static const char *elconErrorMessages[5] = {
		"HW Fail",
		"Charger Overtemp",
		"Wrong Input Volt",
		"No Batt Volt",
		"Comms Timeout"
	};

	static const char *bmsErrorMessages[6] = {
		"Cell Overtemp",
		"Cell Imbalance",
		"Cell Undervolt",
		"Cell Overvolt",
		"Pack Low Volt",
		"Pack High Volt"
	};

	char currentErrors[11][100];
	int currentErrorIndex = 0;

	for (int i = 0; i < 5; i++) {
		if (currentBmsAndElconData.ELCON_fault[i] == 1) {
			sprintf(currentErrors[currentErrorIndex], "%s", elconErrorMessages[i]);
			currentErrorIndex++;
		}
	}
	for (int i = 0; i < 6; i++) {
		if (currentBmsAndElconData.BMS_fault[i] == 1) {
			sprintf(currentErrors[currentErrorIndex], "%s", bmsErrorMessages[i]);
			currentErrorIndex++;
		}
	}

	int numberOfErrors = currentErrorIndex;

	int navBarStartIndex = numberOfErrors;
	int navBarLastIndex = numberOfErrors + 1;
	int numberOfOptions = navBarLastIndex;

	Display_clear();

	Display_drawTitleBar("Errors");

	
	int currentView = selectedOption/3;


	//if the nav bar is selected, ensures that the currentScreen is the last screen of profiles
	if (selectedOption > numberOfErrors - 1) {
		currentView = (numberOfErrors -1 ) / 3;
	}
	int startIndex = currentView * 3;
	//ensures that the correct number of profiles are showed on the last screen
	if (selectedOption > numberOfErrors - 1) {
		startIndex = (numberOfErrors - 1) / 3 * 3;
	}
	//going up from first profile will go to Start button
	if (selectedOption < 0) {
		startIndex = 0;
		selectedOption = navBarLastIndex;
	}
	//going down from start button will reset back to first profile being selected
	if (selectedOption > navBarLastIndex) {
		startIndex = 0;
		selectedOption = 0;
	}

	//initial y-positions used for calculating profile display boxes
	int y1 = 15;
	int y2 = 13;
	int y3 = 24;

	//displays up to three errors per screen
	for (int i = startIndex; i < startIndex + 3 && i < numberOfErrors; i++) {
		ssd1306_SetCursor(3, y1);
		if (selectedOption == i) {
			ssd1306_FillRectangle(1,y2, 122, y3, White);
			ssd1306_WriteString(currentErrors[i], Font_6x8, Black);
		}
		else {
			ssd1306_DrawRectangle(1,y2, 122, y3, White);
			ssd1306_WriteString(currentErrors[i], Font_6x8, White);
		}

		y1 = y1 + 13;
		y2 = y2 + 13;
		y3 = y3 + 13;
	}

	int numberOfViews = (numberOfErrors + 2) / 3; //+ 2 ensures it will always round up

	Display_drawShortScrollBar(currentView, numberOfViews);

	char *navBarOptions[] = {"Nav"};
	int numberOfNavBarOptions = 1;
	Display_drawNavBar(navBarOptions, numberOfNavBarOptions, navBarStartIndex);

	Display_updateScreen();

	if (selectPressed) {
		Display_checkSelectedOptionBounds(numberOfOptions);
		if (selectedOption < navBarStartIndex && selectedOption >= 0 ) {
			return DISPLAY_STATE_ERRORS;
		}
		if (selectedOption == navBarStartIndex) {
			return DISPLAY_STATE_NAVIGATION;
		}
	}
	return DISPLAY_STATE_ERRORS;
}
