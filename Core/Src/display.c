// Written by Ayman Alamayri in Dec 2024
#include "display.h"
#include <stdio.h>

extern int selectedButton;
extern bool selectPressed;
extern int backPressed;
bool isBalancing = false;
bool isBalancingControl = false;

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
};

extern struct bmsAndElconData currentBmsAndElconData;

extern uint16_t LIMIT_VOLTS;
extern uint16_t LIMIT_AMPS;
extern char codeBranch[10];
extern char codeVersion[5];

void DISP_KanoaSplash() {
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	ssd1306_SetCursor(0, 0);
	ssd1306_DrawBitmap(0, 0, kanoaBootImage, 128, 64, White);
	ssd1306_SetCursor(70, 15);
	ssd1306_WriteString("KANOA OS", Font_6x8, White);
	ssd1306_SetCursor(70, 25);
	ssd1306_WriteString(codeVersion, Font_6x8, White);
	ssd1306_SetCursor(70, 35);
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

	char* buttons[] = {"Charging", "Balancing", "Battery", "Exit"};
	// char* buttons[] = {"Home", "Start Charging", "Start Balancing", "Battery", "Charger", "Errors"};
	int numOfButtons = 3;

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


		ssd1306_UpdateScreen();
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
			SRE_Display_Start_Charging();
		}
		else if (selectedButton == 1) {
			SRE_Display_Start_Balancing();
		}
		else if (selectedButton == 2) {
			SRE_Display_Battery1();
		}
		else if (selectedButton == 3) {
			// Restarts the software
			NVIC_SystemReset();
		}
		else if (selectedButton == 3) {
			SRE_Display_Charger_Stats();
		}
		else if (selectedButton ==4) {
			SRE_Display_Err();
		}
	}
}


void SRE_Display_Home() {
	selectedButton = 0;
	selectPressed = false;

	char soc[] = "SOC: 85.7%";
	char charger_temp[] = "Charger Tmp: 100.1C";
	char balancing[] = "Balancing Off";

	int numOfButtons = 2;

	while (!selectPressed) {

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

		char *navBarButtons[] = {"Nav"};
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
			SRE_Display_Battery1();
		}
		else if (selectedButton == 1) {
			SRE_Display_Nav();
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

	ssd1306_UpdateScreen();
}


void SRE_Display_Charging2(){
	char packVoltStats[] = "Pack Volt: 400.22V";
	char soc[50];
	char timeRemaining[] = "Time Remaining: 120m";

	ssd1306_FillRectangle(0, 0, 127, 63, Black);

	SRE_Display_Title_Bar("Charging 2");

	sprintf(soc, "SOC:%.2f%%",
			currentBmsAndElconData.BMS_stateOfCharge);


	ssd1306_SetCursor(1, 13);
	ssd1306_WriteString(packVoltStats, Font_6x8, White);

	//Writes SOC Stats
	ssd1306_SetCursor(1, 23);
	ssd1306_WriteString(soc, Font_6x8, White);

	//Writes timeRemaining
	ssd1306_SetCursor(1, 33);
	ssd1306_WriteString(timeRemaining, Font_6x8, White);

	char *navBarButtons[] = {"Charging 1"};
	SRE_Display_Nav_Bar(navBarButtons, 1, 0);
  
	

	ssd1306_UpdateScreen();

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

	struct Profile p1 = {"P1", 4, 355};
	struct Profile p2 = {"P2", 20, 355};
	struct Profile p3 = {"P3", 3, 385};
	struct Profile p4 = {"P4", 15, 385};
	struct Profile p5 = {"P5", 20, 385};
	struct Profile p6 = {"P6", 10, 400};
	struct Profile p7 = {"P7", 20, 400};
	struct Profile p8 = {"P8", 10, 403};

	int numOfProfiles = 8;
	struct Profile profiles[numOfProfiles];

	profiles[0] = p1;
	profiles[1] = p2;
	profiles[2] = p3;
	profiles[3] = p4;
	profiles[4] = p5;
	profiles[5] = p6;
	profiles[6] = p7;
	profiles[7] = p8;

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



		ssd1306_UpdateScreen();

	}

	if (selectPressed) {
	    // Make sure the selectedButton is within the valid range of profiles
	    if (selectedButton >= 0 && selectedButton < numOfProfiles) {
	        struct Profile selectedProfile = profiles[selectedButton];
	        // Set charging limits based on the selected profile
					LIMIT_VOLTS = selectedProfile.voltage;
					LIMIT_AMPS = selectedProfile.current;
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


		bool errors = false;
		char chargerTempString[25] = "Charger Tmp: 100.22C";


		SRE_Display_Title_Bar("Charger Stats");

		ssd1306_SetCursor(1, 13);
		ssd1306_WriteString(chargerTempString, Font_6x8, White);

		ssd1306_SetCursor(1, 23);

		if (errors) {
			ssd1306_WriteString("Errors detected", Font_6x8, White);
		}
		else {
			ssd1306_WriteString("No errors detected", Font_6x8, White);
		}


		char *navBarButtons[] = {"Batt", "Nav"};
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
			SRE_Display_Battery1();
		}
		else if (selectedButton == 1) {
			SRE_Display_Nav();
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
		
		sprintf(voltageStats, "Vlt H/L:%.2f/%.2fV",
			currentBmsAndElconData.BMS_maxVolt,
			currentBmsAndElconData.BMS_minVolt);

		sprintf(averageStats, "Avg T/V:%.2fC/%.2fV",
			currentBmsAndElconData.BMS_avgTemp,
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
	char balancingOnOff[50];
	char packImbalance[50];

	sprintf(soc, "SOC:%.2f%%",
			currentBmsAndElconData.BMS_stateOfCharge);
			
	sprintf(packImbalance, "Imbalance:%.2fV",
			currentBmsAndElconData.BMS_packImbalance);
	//Below are vertices for the triangle image.
	//Given text starts at y = 33; and is roughly 8px;
	//Write string goes from top to bottom pixel.
	//Goes from 1-10 x, and is 33-40 high;
	uint8_t x1 = 1, y1 = 40;  // Vertex 1
	uint8_t x2 = 5, y2 = 33;  // Vertex 2
	uint8_t x3 = 10, y3 = 40;  // Vertex 3

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

		// Draw the triangle edges
		// ssd1306_Line(x1, y1, x2, y2, White);  // Line from Vertex 1 to Vertex 2
		// ssd1306_Line(x2, y2, x3, y3, White);  // Line from Vertex 2 to Vertex 3
		// ssd1306_Line(x3, y3, x1, y1, White);  // Line from Vertex 3 to Vertex 1

		// ssd1306_SetCursor(15, 33);
		// ssd1306_WriteString(balancingStats, Font_6x8, White);

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
			return;
		}


	}

}

void SRE_Display_Title_Bar(char title[]) {
	ssd1306_SetCursor(1,1);
	ssd1306_WriteString(title, Font_6x8, White);
	ssd1306_Line(0, 10, 127, 10, White);
	SRE_Display_Charger_Symbol(88, 3);
	SRE_Display_Error_Symbol(119,1);
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
	char temperatureStats[50];
	char voltageStats[50];
	char averageStats[50];
	char chargingInfo[30];
	
	sprintf(temperatureStats, "Tmp H/L:%.2f/%.2fC",
			currentBmsAndElconData.BMS_maxTemp,
			currentBmsAndElconData.BMS_minTemp);
	
	sprintf(voltageStats, "Vlt H/L:%.2f/%.2fV",
			currentBmsAndElconData.BMS_maxVolt,
			currentBmsAndElconData.BMS_minVolt);

	sprintf(averageStats, "Avg T/V:%.2fC/%.2fV",
			currentBmsAndElconData.BMS_avgTemp,
			currentBmsAndElconData.BMS_avgVolt);

	sprintf(chargingInfo, "%d volts @ %d amps", 
			LIMIT_VOLTS, LIMIT_AMPS);

	//Resets screen
	ssd1306_FillRectangle(0, 0, 127, 63, Black);

	//Writes "Charging 1"
	SRE_Display_Title_Bar("Charging 1");

	//Writes temp
	ssd1306_SetCursor(1, 13);
	ssd1306_WriteString(temperatureStats, Font_6x8, White);

	//Writes voltage
	ssd1306_SetCursor(1, 23);
	ssd1306_WriteString(voltageStats, Font_6x8, White);

	//Writes averageStats
	ssd1306_SetCursor(1, 33);
	ssd1306_WriteString(averageStats, Font_6x8, White);

	//Writes charging info
	ssd1306_SetCursor(1, 43);
	ssd1306_WriteString(chargingInfo, Font_6x8, White);

	char *navBarButtons[] = {"Charging 2"};
	SRE_Display_Nav_Bar(navBarButtons, 1, 0);

	ssd1306_UpdateScreen();
}

void SRE_Display_Err() {
	selectedButton = 0;
	selectPressed = false;

	char err1[] = "Error message 1";
	char err2[] = "Error message 2";
	char err3[] = "Error message 3";

	int numOfButtons = 2;


	while (!selectPressed) {
		ssd1306_FillRectangle(0, 0, 127, 63, Black);

		if (selectedButton > numOfButtons-1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = numOfButtons-1;
		}

		SRE_Display_Title_Bar("Errors");

		ssd1306_SetCursor(3, 15);
		ssd1306_WriteString(err1, Font_6x8, White);
		ssd1306_DrawRectangle(1, 13, 122, 23, White);

		ssd1306_SetCursor(3, 28);
		ssd1306_WriteString(err2, Font_6x8, White);
		ssd1306_DrawRectangle(1, 26, 122, 36, White);

		ssd1306_SetCursor(3, 41);
		ssd1306_WriteString(err3, Font_6x8, White);
		ssd1306_DrawRectangle(1, 39, 122, 49, White);

		char *navBarButtons[] = {"Batt", "Nav"};
		SRE_Display_Nav_Bar(navBarButtons, 2, 0);

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
			SRE_Display_Battery1();
		}
		else if (selectedButton == 1) {
			SRE_Display_Nav();
		}
	}
}
