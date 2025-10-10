// Written by Ayman Alamayri in Dec 2024
#include "display.h"
#include <stdio.h>

extern int selectedButton;
extern bool selectPressed;
extern int backPressed;
bool isBalancing = false;
bool isBalancingControl = false;
bool isError = false;
int currentChargingScreen = 1;
extern bool isCharging;
extern uint16_t *therm_inlet;
extern uint16_t *therm_outlet;
extern uint16_t THERM_RESIST;
extern uint16_t MAX_ALLOWED_PWR;
extern float READ_THERM(uint16_t *adc_thermistor, uint16_t therm_first_resistance);
bool isChargingSequence = false;

struct bmsAndElconData {
    float BMS_avgVolt;
	float BMS_sumOfCells;
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
};

extern struct bmsAndElconData currentBmsAndElconData;

extern float LIMIT_VOLTS;
extern float LIMIT_AMPS;
extern char codeBranch[10];
extern char codeVersion[5];

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

// Initialization function
void SRE_Display_Init(bool test_mode) {
	ssd1306_Init();
	if (test_mode) {
		SRE_Display_Test();
	}
}
// Function to test display
void SRE_Display_Test() {
	//ssd1306_Fill(White);
	SRE_Display_Nav();
	ssd1306_UpdateScreen();
}
// Example function to display navigation
void SRE_Display_Nav() {
	selectedButton = 0;
	selectPressed = false;

	static uint32_t lastDisplayUpdate = 0;  // Rate limit timer



	char* buttons[] = {"Home", "Charging", "Balancing", "Battery", "Charger Stats", "Errors", "Restart"};
	// char* buttons[] = {"Home", "Start Charging", "Start Balancing", "Battery", "Charger", "Errors"};
	int numOfButtons = 7;

	while(!selectPressed) {
		ssd1306_FillRectangle(0, 0, 127, 63, Black);

		if (selectedButton > numOfButtons-1){
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = numOfButtons-1;
		}

		int y1 = 15;
		int y2 = 13;
		int y3 = 24;

		//Navigation title
		SRE_Display_Title_Bar("Navigation");

		int currentScreen = selectedButton/4;
		int startIndex = currentScreen*4;

		for (int i = startIndex; i < startIndex + 4 && i < numOfButtons; i++) {
			ssd1306_SetCursor(3, y1);
			if (selectedButton == i) {
				ssd1306_FillRectangle(1,y2, 122, y3, White);
				ssd1306_WriteString(buttons[i], Font_6x8, Black);
			}
			else {
				ssd1306_DrawRectangle(1,y2, 122, y3, White);
				ssd1306_WriteString(buttons[i], Font_6x8, White);
			}

			y1 = y1 + 13;
			y2 = y2 + 13;
			y3 = y2 + 10;
		}

		int numOfScreens = (numOfButtons+3)/4;
		SRE_Display_Long_Scroll_Bar(currentScreen, numOfScreens);

		uint32_t currentTime = HAL_GetTick();
        if (currentTime - lastDisplayUpdate >= 300) {
            extern I2C_HandleTypeDef hi2c2;
            if (HAL_I2C_GetState(&hi2c2) == HAL_I2C_STATE_READY) {
                ssd1306_UpdateScreen();  
                lastDisplayUpdate = currentTime;
            }
        }
	}

	if (selectPressed) {
		if (selectedButton > numOfButtons-1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = numOfButtons;
		}

		// Populate with the function name that corresponds to each button number respectively later.
		if (selectedButton == 0) {
			SRE_Display_Home();
		}
		else if (selectedButton == 1) {
			SRE_Display_Start_Charging();
		}
		else if (selectedButton == 2) {
			// Restarts the software
			SRE_Display_Start_Balancing();
		}
		else if (selectedButton == 3) {
			SRE_Display_Battery1();
		}
		else if (selectedButton == 4) {
			SRE_Display_Charger_Stats();
		}
		else if (selectedButton == 5) {
			SRE_Display_Err();
		}
		else if (selectedButton ==6) {
			NVIC_SystemReset();
		}
	}
}


void SRE_Display_Home() {
	selectedButton = 0;
	selectPressed = false;

	

	int numOfButtons = 2;

	while (!selectPressed) {

		char soc[50];
		char chargerTemp[50];
		char balancing[50];

		sprintf(soc, "SOC:%.2f%%",
				currentBmsAndElconData.BMS_stateOfCharge);

		//TODO READ CHARGER TEMP
		char charger_temp[] = "Charger Tmp: 100.1C";
		

		sprintf(balancing, "Balancing %s", isBalancing ? "On" : "Off");

		ssd1306_FillRectangle(0, 0, 127, 63, Black);

		if (selectedButton > numOfButtons-1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = numOfButtons-1;
		}

		SRE_Display_Title_Bar("Home");

		ssd1306_SetCursor(1, 13);
		ssd1306_WriteString(soc, Font_6x8, White);

		ssd1306_SetCursor(1, 23);
		ssd1306_WriteString(charger_temp, Font_6x8, White);

		ssd1306_SetCursor(1, 33);
		ssd1306_WriteString(balancing, Font_6x8, White);

		char *navBarButtons[] = {"Nav", "Batt"};
		SRE_Display_Nav_Bar(navBarButtons, 1, 0);

		ssd1306_UpdateScreen();
	}

	if (selectPressed) {
		selectPressed = false;

		if (selectedButton > numOfButtons-1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = numOfButtons-1;
		}

		if (selectedButton == 0) {
			SRE_Display_Nav();
		}
		else if (selectedButton == 1) {
			SRE_Display_Battery1();
		}
	}

}

void SRE_Display_Charging_Instructions() {
	selectedButton = 0;

	char instruct[] = "How to Charge";
	char step1[] = "Press the red button";
	char step2[] = "Placeholder";
	char step3[] = "Placeholder 2";


	// [todo] Make detection to check if step instruction is completed -> Go to new screen
	// [todo] Cancel -> goes to some page
	// [todo] Once finished, goes to Charger 1 stats

	while (!selectPressed) {
		ssd1306_FillRectangle(0, 0, 127, 63, Black);

		if (selectedButton >= 1 || selectedButton < 0) {
			selectedButton = 0;
		}

		SRE_Display_Title_Bar("How to Charge");

		ssd1306_SetCursor(1, 13);
		ssd1306_WriteString(step1, Font_16x15, White);

		ssd1306_SetCursor(1, 22);
		ssd1306_WriteString(step2, Font_16x15, White);

		ssd1306_SetCursor(1, 31);
		ssd1306_WriteString(step3, Font_16x15, White);

		char *navBarButtons[] = {"Cancel"};
		SRE_Display_Nav_Bar(navBarButtons, 1, 0);

		ssd1306_UpdateScreen();
	}

	if (selectPressed) {
		selectPressed = false;

		if (selectedButton >= 1 || selectedButton < 0) {
			selectedButton = 0;
		}

		// Goes to some page after abort
	}
}

void SRE_Display_Nav_Bar(char *buttons[], int numOfButtons, int firstButtonIndex) {

	//the selectedButton values for nav bar will vary based on currenty displayed screen
	int maxSelectedButtonIndex = firstButtonIndex + numOfButtons-1;
	int buttonIndex = firstButtonIndex;

	int x1 = 1;
	int x2 = 1;

	for (int i = 0; i < numOfButtons; i++) {
		x2 = x1 + (strlen(buttons[i]) * 6) + 2;
		if (selectedButton == buttonIndex ||
			(buttonIndex == 0 && selectedButton > maxSelectedButtonIndex) ||
			(buttonIndex == maxSelectedButtonIndex && selectedButton < 0))
		{
			ssd1306_FillRectangle(x1, 52, x2, 62, White);
			ssd1306_SetCursor(x1 + 2, 54);
			ssd1306_WriteString(buttons[i], Font_6x8, Black);
		}
		else {
			ssd1306_DrawRectangle(x1, 52, x2, 62, White);
			ssd1306_SetCursor(x1 + 2, 54);
			ssd1306_WriteString(buttons[i], Font_6x8, White);
		}

		buttonIndex++;
		x1 = x2 + 2;
	}
}


void SRE_Display_Charging2() {
	int numOfButtons = 1;
  //"Pack Volt: 400.22V"

	char sumOfCells[50];
	char soc[50];
	char averageStats[50];
	char chargingInfo[50];

	sprintf(averageStats, "Avg V:%.3fV", currentBmsAndElconData.BMS_avgVolt);

	sprintf(chargingInfo, "%.2f V @ %.2f A", LIMIT_VOLTS, LIMIT_AMPS);

	sprintf(soc, "SOC:%.2f%%", currentBmsAndElconData.BMS_stateOfCharge);

	sprintf(sumOfCells, "Pack Volt: %.2fV", currentBmsAndElconData.BMS_sumOfCells);

	ssd1306_FillRectangle(0, 0, 127, 63, Black);

	if (isBalancing) {
		SRE_Display_Title_Bar("Balancing 2");
	}
	else if (!isBalancing) {
		SRE_Display_Title_Bar("Charging 2");
	}

	



	ssd1306_SetCursor(1, 13);
	ssd1306_WriteString(sumOfCells, Font_6x8, White);

	//Writes SOC Stats
	ssd1306_SetCursor(1, 23);
	ssd1306_WriteString(soc, Font_6x8, White);

	ssd1306_SetCursor(1, 33);
	ssd1306_WriteString(averageStats, Font_6x8, White);

	ssd1306_SetCursor(1, 43);
	ssd1306_WriteString(chargingInfo, Font_6x8, White);


	char *navBarButtons[1];
	if (isBalancing) {
		navBarButtons[0] = "Balancing 1";
	} else {
		navBarButtons[0] = "Charging 1";
	}
	SRE_Display_Nav_Bar(navBarButtons, 1, 0);

	if (selectPressed) {
		selectPressed = false;
		if (selectedButton < 0) {
			selectedButton = 0;
		}
		else if (selectedButton > numOfButtons-1) {
			selectedButton = numOfButtons-1;
		}
		if (selectedButton == 0) {
			currentChargingScreen = 1;
		}
	}

}

void SRE_Display_Start_Charging() {
	selectPressed = false;
	selectedButton = 0;

	//sets up sample profiles to use for testing
	struct Profile {
		char name[5];
		uint16_t current;
		uint16_t voltage;
	};

	struct Profile allProfiles[] = {
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

	int numOfProfiles = 9;
	struct Profile profiles[numOfProfiles];
	
	int profile_index = 0;
	for (int i = 0; i < numOfProfiles; i++) {
		if (allProfiles[i].voltage * allProfiles[i].current <= (MAX_ALLOWED_PWR * 97 / 100) && allProfiles[i].voltage > currentBmsAndElconData.BMS_sumOfCells) {
			profiles[profile_index] = allProfiles[i];
			profile_index++;
		}
	}

	numOfProfiles = profile_index;

  	int navStartIndex = numOfProfiles;
  	int navLastIndex = numOfProfiles;

	while (!selectPressed) {

		//resets screen
		ssd1306_FillRectangle(0,0,127,63, Black);

		SRE_Display_Title_Bar("Charging");

		int currentScreen = selectedButton/3;


		//if the nav bar is selected, ensures that the currentScreen is the last screen of profiles
		if (selectedButton > numOfProfiles-1) {
			currentScreen = (numOfProfiles-1)/3;
		}
		int startIndex = currentScreen*3;
		//ensures that the correct number of profiles are showed on the last screen
		if (selectedButton > numOfProfiles-1) {
			startIndex = (numOfProfiles-1)/3*3;
		}
		//going up from first profile will go to Start button
		if (selectedButton < 0) {
			startIndex = 0;
			selectedButton = navLastIndex;
		}
		//going down from start button will reset back to first profile being selected
		if (selectedButton > navLastIndex) {
			startIndex = 0;
			selectedButton = 0;
		}

		//initial y-positions used for calculating profile display boxes
		int y1 = 15;
		int y2 = 13;
		int y3 = 24;

		//displays up to three profiles per screen
		for (int i = startIndex; i < startIndex + 3 && i < numOfProfiles; i++) {
			char profileString[50];
			sprintf(profileString, "%s: %dA %dV", profiles[i].name, profiles[i].current, profiles[i].voltage);
			ssd1306_SetCursor(3, y1);

			if (selectedButton == i) {
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
		int numOfScreens = (numOfProfiles+2)/3;

		SRE_Display_Short_Scroll_Bar(currentScreen, numOfScreens);

		char *navBarButtons[] = {"Nav"};
		SRE_Display_Nav_Bar(navBarButtons,1, navStartIndex);

		static uint32_t lastDisplayUpdate = 0;  // Rate limit timer
		uint32_t currentTime = HAL_GetTick();
        if (currentTime - lastDisplayUpdate >= 300) {
            extern I2C_HandleTypeDef hi2c2;
            if (HAL_I2C_GetState(&hi2c2) == HAL_I2C_STATE_READY) {
                ssd1306_UpdateScreen();  
                lastDisplayUpdate = currentTime;
            }
        }
	}

	if (selectPressed) {
	    // Make sure the selectedButton is within the valid range of profiles
	    if (selectedButton >= 0 && selectedButton < numOfProfiles) {
	        struct Profile selectedProfile = profiles[selectedButton];
	        // Set charging limits based on the selected profile
					LIMIT_VOLTS = selectedProfile.voltage;
					LIMIT_AMPS = selectedProfile.current;
					isChargingSequence = true;
					selectPressed = false;
					selectedButton = 0;
					return;  // Exit after setting the limits
	    }
	    else if (selectedButton == numOfProfiles) {
			SRE_Display_Nav();
	    }
	}
}

void SRE_Display_Short_Scroll_Bar(int currentScreen, int numOfScreens) {

	//currentScreen is zero-indexed

	if (numOfScreens > 1) {

		int scrollContainerHeight = 35;
		int scrollBarLength = scrollContainerHeight/numOfScreens;

		int scrollBarStart = 14+ (currentScreen*scrollBarLength);

		ssd1306_DrawRectangle(124, 13, 126, 47, White);
		ssd1306_Line(125, scrollBarStart, 125, scrollBarStart+scrollBarLength, White);
	}

}

void SRE_Display_Long_Scroll_Bar(int currentScreen, int numOfScreens) {

	//currentScreen is zero-indexed

	if (numOfScreens > 1) {

		int scrollContainerHeight = 49;
		int scrollBarLength = scrollContainerHeight/numOfScreens;

		int scrollBarStart = 14+ (currentScreen*scrollBarLength);

		ssd1306_DrawRectangle(124, 13, 126, 62, White);
		ssd1306_Line(125, scrollBarStart, 125, scrollBarStart+scrollBarLength, White);
	}

}

void SRE_Display_Charger_Stats() {
	selectPressed = false;
	selectedButton = 0;

	int numOfButtons = 2;

	while (!selectPressed) {

		ssd1306_FillRectangle(0, 0, 127, 63, Black);


		if (selectedButton > numOfButtons-1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = numOfButtons-1;
		}

		char inletTempString[50];
		char outletTempString[50];

		sprintf(inletTempString, "Inlet Tmp:%.2f", READ_THERM(therm_inlet, THERM_RESIST));
		sprintf(outletTempString, "Outlet Tmp:%.2f", READ_THERM(therm_outlet, THERM_RESIST));

		SRE_Display_Title_Bar("Charger Stats");

		ssd1306_SetCursor(1, 13);
		ssd1306_WriteString(inletTempString, Font_6x8, White);

		ssd1306_SetCursor(1, 23);
		ssd1306_WriteString(outletTempString, Font_6x8, White);

		ssd1306_SetCursor(1, 33);
		if (isError) {
			ssd1306_WriteString("Errors detected", Font_6x8, White);
		}
		else {
			ssd1306_WriteString("No errors detected", Font_6x8, White);
		}


		char *navBarButtons[] = {"Nav", "Batt"};
		SRE_Display_Nav_Bar(navBarButtons, numOfButtons, 0);

		ssd1306_UpdateScreen();
	}

	if (selectPressed) {
		if (selectedButton > numOfButtons-1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = numOfButtons-1;
		}

		if (selectedButton == 0) {
			SRE_Display_Nav();
		}
		else if (selectedButton == 1) {
			SRE_Display_Battery1();
		}
	}
}

void SRE_Display_Battery1(){
	selectPressed = false;
	selectedButton = 0;
	
	

	int numOfButtons = 2;

	while(!selectPressed){
		char temperatureStats[50];
		char voltageStats[50];
		char averageStats[50];
	
		sprintf(temperatureStats, "Tmp H/L:%.2f/%.2fC",
			currentBmsAndElconData.BMS_maxTemp,
			currentBmsAndElconData.BMS_minTemp);
		
		sprintf(voltageStats, "Vlt H/L:%.3f/%.3fV",
			currentBmsAndElconData.BMS_maxVolt,
			currentBmsAndElconData.BMS_minVolt);

		sprintf(averageStats, "Avg V:%.3fV",
			currentBmsAndElconData.BMS_avgVolt);

		ssd1306_FillRectangle(0, 0, 127, 63, Black);

		if (selectedButton > numOfButtons-1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = numOfButtons-1;
		}

		SRE_Display_Title_Bar("Battery 1");

		//Writes temp
		ssd1306_SetCursor(1, 13);
		ssd1306_WriteString(temperatureStats, Font_6x8, White);
		//Writes voltage
		ssd1306_SetCursor(1, 23);
		ssd1306_WriteString(voltageStats, Font_6x8, White);
		//Writes averageStats
		ssd1306_SetCursor(1, 33);
		ssd1306_WriteString(averageStats, Font_6x8, White);

		//Writes button for Nav and selects.
		char *navButtons[] = {"Nav", "Battery 2"};
		SRE_Display_Nav_Bar(navButtons, 2, 0);

		ssd1306_UpdateScreen();
	}

	if (selectPressed) {
		if (selectedButton > numOfButtons-1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = numOfButtons-1;
		}

		if (selectedButton == 0) {
			SRE_Display_Nav();
		}
		else if (selectedButton ==1) {
			SRE_Display_Battery2();
		}

	}



}


void SRE_Display_Battery2(){
	selectPressed = false;
	selectedButton = 0;

	char soc[50];
	char packVolt[50];
	char packImbalance[50];

	sprintf(soc, "SOC:%.2f%%",
			currentBmsAndElconData.BMS_stateOfCharge);
			
	sprintf(packImbalance, "Imbalance:%.2fV",
			currentBmsAndElconData.BMS_packImbalance);
	
	sprintf(packVolt, "Pack Volt: %.2fV",
			currentBmsAndElconData.BMS_sumOfCells);

	//char balancingStats[] = "Balancing: 20.22V";

	int numOfButtons = 2;

	while(!selectPressed) {

		ssd1306_FillRectangle(0, 0, 127, 63, Black);

		if (selectedButton > numOfButtons-1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = numOfButtons-1;
		}
		SRE_Display_Title_Bar("Battery 2");

		ssd1306_SetCursor(1, 13);
		ssd1306_WriteString(soc, Font_6x8, White);

		ssd1306_SetCursor(1, 23);
		ssd1306_WriteString(packImbalance, Font_6x8, White);

		ssd1306_SetCursor(1, 33);
		ssd1306_WriteString(packVolt, Font_6x8, White);

		
		char *navButtons[] = {"Nav", "Battery 1"};
		SRE_Display_Nav_Bar(navButtons, 2, 0);

		ssd1306_UpdateScreen();
	}

	if (selectPressed) {
		if (selectedButton > numOfButtons-1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = numOfButtons-1;
		}

		if (selectedButton == 0) {
			SRE_Display_Nav();
		}
		else if (selectedButton == 1) {
			SRE_Display_Battery1();
		}
	}

}

//Display Start Balancing
void SRE_Display_Start_Balancing(){
	selectPressed = false;
	selectedButton = 0;

	char balancingOnOff[] = "Balancing is off";


	//NOTE: Parameters of drawLine and rectangle may be off. Might need to set Cursor
		//for them also before calling them.
		//Can't really test without working OLED.
	//Writes "Charging 1"
		//Change to (1,2) probably

	int numOfButtons = 2;

	while(!selectPressed){


		if (selectedButton > numOfButtons-1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = numOfButtons-1;
		}

		ssd1306_FillRectangle(0, 0, 127, 63, Black);

		SRE_Display_Title_Bar("Balancing");

		ssd1306_SetCursor(1, 13);
		ssd1306_WriteString(balancingOnOff, Font_6x8, White);

		char *navButtons[] = {"Nav", "Start Bal"};
		SRE_Display_Nav_Bar(navButtons, numOfButtons, 0);


		ssd1306_UpdateScreen();
	}
	if (selectPressed) {
		if (selectedButton > numOfButtons-1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = numOfButtons-1;
		}

		if (selectedButton == 0) {
			SRE_Display_Nav();
		}
		else if (selectedButton == 1) {
			isBalancing = true;
			isBalancingControl = true;
			isChargingSequence = true;
			selectPressed = false;
			selectedButton = 0;
			return;
		}


	}

}

void SRE_Display_Title_Bar(char title[]) {
	static uint32_t previous_time = 0;

	uint32_t current_time = HAL_GetTick();
	if (current_time - previous_time >= 150) {
		ssd1306_DrawPixel(85, 5, White);
		previous_time = current_time;
	}
	else {
		ssd1306_DrawPixel(85, 5, Black);
	}

	isError = false;
	for (int i = 0; i < 5; i++) {
		if (currentBmsAndElconData.ELCON_fault[i] == 1) {
			isError = true;
			break;
		}
	}

	ssd1306_SetCursor(1, 1);
	ssd1306_WriteString(title, Font_6x8, White);
	ssd1306_Line(0, 10, 127, 10, White);

	//Flashing status symbols 
	// ssd1306_FillRectangle(70, 0, 127, 9, Black);
	// ssd1306_UpdateScreen();

	if (isError) {
		SRE_Display_Error_Symbol(119,1);
		if (isCharging) {
			SRE_Display_Charger_Symbol(92, 3);
			if (isBalancing) {
				ssd1306_FillRectangle(71, 0, 89, 8, White);
				ssd1306_SetCursor(72, 1);
				ssd1306_WriteString("BAL", Font_6x8, Black);
			}
		}
		else if (isBalancing) {
			ssd1306_FillRectangle(91, 0, 109, 8, White);
			ssd1306_SetCursor(92, 1);
			ssd1306_WriteString("BAL", Font_6x8, Black);
		}
	}
	else if (isCharging) {
		SRE_Display_Charger_Symbol(109, 3);
		if (isBalancing) {
			ssd1306_FillRectangle(88, 0, 106, 8, White);
			ssd1306_SetCursor(89, 1);
			ssd1306_WriteString("BAL", Font_6x8, Black);
		}
	}
	else if (isBalancing) {
		ssd1306_FillRectangle(108, 0, 126, 8, White);
		ssd1306_SetCursor(109, 1);
		ssd1306_WriteString("BAL", Font_6x8, Black);
	}
}

void SRE_Display_Charger_Symbol(int x, int y) {
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

void SRE_Display_Error_Symbol(int x, int y) {
	//point of origin (x,y) is the top of the triangle
	ssd1306_Line(x, y, x+7, y+7, White);
	ssd1306_Line(x, y, x-7, y+7, White);
	ssd1306_Line(x-7, y+7, x+7, y+7, White);

	ssd1306_Line(x, y+2, x, y+4, White);
	ssd1306_Line(x, y+6, x, y+6, White);
}

void SRE_Display_Charging1() {

	int numOfButtons = 1;
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



	//Resets screen
	ssd1306_FillRectangle(0, 0, 127, 63, Black);

	//Writes title
	if (isBalancing) {
		SRE_Display_Title_Bar("Balancing 1");
	}
	else if (!isBalancing) {
		SRE_Display_Title_Bar("Charging 1");
	}

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
	
	char *navBarButtons[1];
	if (isBalancing) {
		navBarButtons[0] = "Balancing 2";
	} else {
		navBarButtons[0] = "Charging 2";
	}
	SRE_Display_Nav_Bar(navBarButtons, 1, 0);

	if (selectPressed) {
		selectPressed = false;
		if (selectedButton < 0) {
			selectedButton = 0;
		}
		else if (selectedButton > numOfButtons-1) {
			selectedButton = numOfButtons-1;
		}
		if (selectedButton == 0) {
			currentChargingScreen = 2;
		}
	}
}

void SRE_Display_Err() {
	selectedButton = 0;
	selectPressed = false;
	int navStartIndex;
	int navLastIndex;

	while (!selectPressed) {
		const char *error_messages[5] = {
					"HW Fail",
					"Charger Overtemp",
					"Wrong Input Volt",
					"No Batt Volt",
					"Comms Timeout"
		};

		char current_errors[5][100];
		int current_error_index = 0;

		for (int i = 0; i < 5; i++) {
		
      
			if (currentBmsAndElconData.ELCON_fault[i] == 1) {
				sprintf(current_errors[current_error_index], "%s", error_messages[i]);
				current_error_index++;
			}
		}

		int numOfErrors = current_error_index;

		navStartIndex = numOfErrors;
		navLastIndex = numOfErrors+1;

		ssd1306_FillRectangle(0, 0, 127, 63, Black);

		SRE_Display_Title_Bar("Errors");

		
		int currentScreen = selectedButton/3;


		//if the nav bar is selected, ensures that the currentScreen is the last screen of profiles
		if (selectedButton > numOfErrors-1) {
			currentScreen = (numOfErrors-1)/3;
		}
		int startIndex = currentScreen*3;
		//ensures that the correct number of profiles are showed on the last screen
		if (selectedButton > numOfErrors-1) {
			startIndex = (numOfErrors-1)/3*3;
		}
		//going up from first profile will go to Start button
		if (selectedButton < 0) {
			startIndex = 0;
			selectedButton = navLastIndex;
		}
		//going down from start button will reset back to first profile being selected
		if (selectedButton > navLastIndex) {
			startIndex = 0;
			selectedButton = 0;
		}

		//initial y-positions used for calculating profile display boxes
		int y1 = 15;
		int y2 = 13;
		int y3 = 24;

		//displays up to three errors per screen
		for (int i = startIndex; i < startIndex + 3 && i < numOfErrors; i++) {
			ssd1306_SetCursor(3, y1);
			if (selectedButton == i) {
				ssd1306_FillRectangle(1,y2, 122, y3, White);
				ssd1306_WriteString(current_errors[i], Font_6x8, Black);
			}
			else {
				ssd1306_DrawRectangle(1,y2, 122, y3, White);
				ssd1306_WriteString(current_errors[i], Font_6x8, White);
			}

			y1 = y1 + 13;
			y2 = y2 + 13;
			y3 = y3 + 13;
		}

		//numOfErrors + 2 ensures it will always round up
		int numOfScreens = (numOfErrors+2)/3;

		SRE_Display_Short_Scroll_Bar(currentScreen, numOfScreens);

		char *navBarButtons[] = {"Nav", "Batt"};
		SRE_Display_Nav_Bar(navBarButtons, 2, navStartIndex);

		ssd1306_UpdateScreen();
	}

	if (selectPressed) {
		if (selectedButton < 0) {
			selectedButton = 0;
		}
    if (selectedButton > navLastIndex) {
			selectedButton = navLastIndex;
		}
		if (selectedButton < navStartIndex && selectedButton >= 0 ) {
			SRE_Display_Err();
		}
		if (selectedButton == navStartIndex) {
			SRE_Display_Nav();
		}
		if (selectedButton == navStartIndex + 1) {
			SRE_Display_Battery1();
		}
	}
}
