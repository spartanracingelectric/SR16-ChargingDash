// Written by Ayman Alamayri in Dec 2024
#include <stdbool.h>
#include "display.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <string.h>
#include <stdio.h>
extern int selectedButton;
extern bool selectPressed;
extern int backPressed;

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
	char *navButtons[] = {"Nav", "Batt", "Start"};
	SRE_Display_Nav_Bar(navButtons, 3, 0);
	ssd1306_UpdateScreen();
}
// Example function to display navigation
void SRE_Display_Nav() {
	selectedButton = 0;

	char nav[] = "Navigation";
	char home[] = "Home";
	char charge[] = "Start Charging";
	char balance[] = "Start Balancing";
	char battery[] = "Battery";

	char retval;

	// 4 buttons: home, charging, balancing, battery
	while(!selectPressed) {
		ssd1306_FillRectangle(0, 0, 127, 63, Black);

		if (selectedButton > 3){
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = 3;
		}

		//Navigation title
		ssd1306_SetCursor(1, 1);
		retval = ssd1306_WriteString(nav, Font_16x15, White);

		ssd1306_Line(0, 10, 127, 10, White);

		// Home
		if (selectedButton == 0 || selectedButton > 4) {
			ssd1306_SetCursor(3, 15);
			retval = ssd1306_WriteString(home, Font_16x15, Black);
			ssd1306_FillRectangle(1, 13, 122, 11, White);
		}
		else {
			ssd1306_SetCursor(3, 15);
			retval = ssd1306_WriteString(home, Font_16x15, White);
			ssd1306_DrawRectangle(1, 13, 122, 11, White);
		}

		// Charging
		if (selectedButton == 1) {
			ssd1306_SetCursor(3, 28);
			retval = ssd1306_WriteString(charge, Font_16x15, Black);
			ssd1306_FillRectangle(1, 26, 122, 11, White);
		}
		else {
			ssd1306_SetCursor(3, 28);
			retval = ssd1306_WriteString(charge, Font_16x15, White);
			ssd1306_DrawRectangle(1, 26, 122, 11, White);
		}

		// Balancing
		if (selectedButton == 2) {
			ssd1306_SetCursor(3, 41);
			retval = ssd1306_WriteString(balance, Font_16x15, Black);
			ssd1306_FillRectangle(1, 39, 122, 11, White);
		}
		else {
			ssd1306_SetCursor(3, 41);
			retval = ssd1306_WriteString(balance, Font_16x15, White);
			ssd1306_DrawRectangle(1, 39, 122, 11, White);
		}

		// Battery
		if (selectedButton == 3 || selectedButton < 0) {
			ssd1306_SetCursor(3, 54);
			retval = ssd1306_WriteString(battery, Font_16x15, Black);
			ssd1306_FillRectangle(1, 52, 122, 11, White);
		}
		else {
			ssd1306_SetCursor(3, 54);
			retval = ssd1306_WriteString(battery, Font_16x15, White);
			ssd1306_DrawRectangle(1, 52, 122, 11, White);
		}

		ssd1306_UpdateScreen();
	}

	if (selectPressed) {
		selectPressed = false;

		if (selectedButton > 3) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = 1;
		}

		// Populate with the function name that corresponds to each button number respectively later.
		if (selectedButton == 0) {
			// Goes to home
		}
		else if (selectedButton == 1) {
			// Goes to Charging

		}
		else if (selectedButton == 2) {
			// Goes to Balancing

		}
		else if (selectedButton == 3) {
			// Goes to Battery

		}
	}
}
}

void SRE_Display_Home() {
	selectedButton = 0;

	char home[] = "Home";
	char soc[] = "SOC: 85.7%";
	char charger_temp[] = "Charger Tmp: 100.1C";
	char balancing[] = "Balancing Off";
	char retval;

	while (!selectPressed) {

		ssd1306_FillRectangle(0, 0, 127, 63, Black);

		if (selectedButton > 1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = 1;
		}

		ssd1306_SetCursor(1, 1);
		retval = ssd1306_WriteString(home, Font_16x15, White);
		ssd1306_Line(0, 10, 127, 10, White);

		ssd1306_SetCursor(1, 13);
		retval = ssd1306_WriteString(soc, Font_16x15, White);

		ssd1306_SetCursor(1, 23);
		retval = ssd1306_WriteString(charger_tmp, Font_16x15, White);

		ssd1306_SetCursor(1, 33);
		retval = ssd1306_WriteString(balancing, Font_16x15, White);

		char *navBarButtons[] = {"Batt", "Nav"};
		SRE_Display_Nav_Bar(navBarButtons, 2, 0);

		ssd1306_UpdateScreen();
	}

	if (selectPressed) {
		selectPressed = false;

		if (selectedButton > 1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = 1;
		}

		if (selectedButton == 0) {
			// Goes to Batt
		}
		else if (selectedButton == 1) {
			// Goes to Nav
		}
	}

}

void SRE_Display_Charging_Instructions() {
	selectedButton = 0;

	char instruct[] = "How to Charge";
	char step1[] = "Press the red button";
	char step2[] = "Placeholder";
	char step3[] = "Placeholder 2";

	char retval;

	// [todo] Make detection to check if step instruction is completed -> Go to new screen
	// [todo] Cancel -> goes to some page
	// [todo] Once finished, goes to Charger 1 stats

	while (!selectPressed) {
		ssd1306_FillRectangle(0, 0, 127, 63, Black);
		if (selectedButton >= 1 || selectedButton < 0) {
			selectedButton = 0;
		}

		ssd1306_SetCursor(1, 1);
		retval = ssd1306_WriteString(instruct, Font_16x15, White);
		ssd1306_Line(0, 10, 127, 10, White);

		ssd1306_SetCursor(1, 13);
		retval = ssd1306_WriteString(step1, Font_16x15, White);

		ssd1306_SetCursor(1, 22);
		retval = ssd1306_WriteString(step2, Font_16x15, White);

		ssd1306_SetCursor(1, 31);
		retval = ssd1306_WriteString(step3, Font_16x15, White);

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
	char charging2Title[] = "Charging 2";
	char packVoltStats[] = "Pack Volt: 400.22V";
	char socStats[] = "SOC: 92.7%";
	char timeRemaining[] = "Time Remaining: 120m";
	char charging1Button[] = "Charging 1";

	//Writes "Charging 1"
	ssd1306_SetCursor(1, 1);
	ssd1306_WriteString(charging2Title, Font_6x8, White);
	ssd1306_Line(0, 10, 127, 10, White);

	//Everything appears to be incremented by Y = 10, so font is roughly 8 squares.
	//Writes Pack Volt Stats
	ssd1306_SetCursor(1, 13);
	ssd1306_WriteString(packVoltStats, Font_6x8, White);

	//Writes SOC Stats
	ssd1306_SetCursor(1, 23);
	ssd1306_WriteString(socStats, Font_6x8, White);

	//Writes timeRemaining
	ssd1306_SetCursor(1, 33);
	ssd1306_WriteString(timeRemaining, Font_6x8, White);
  
	//Writes charging1 button
		//Increase in 2 x and 1 y because box takes extra space.
	ssd1306_SetCursor(3, 54);
	ssd1306_WriteString(charging1Button, Font_6x8, Black);
		//draws rectangle surrounding next text
		//x1, y1, x2, y2:
	ssd1306_DrawRectangle(1, 52, 64, 63, White);

	ssd1306_UpdateScreen();

}

void SRE_Display_Start_Charging() {

	//sets up sample profiles to use for testing
	struct Profile {
		char name[5];
		int current;
		int voltage;
		bool isBalancing;
	};

	struct Profile p1 = {"P1", 10, 20, true};
	struct Profile p2 = {"P2", 10, 400, true};
	struct Profile p3 = {"P3", 10, 15, false};
	struct Profile p4 = {"P4", 10, 20, true};
	struct Profile p5 = {"P5", 10, 400, true};
	struct Profile p6 = {"P6", 10, 15, false};
	struct Profile p7 = {"P7", 10, 400, true};
	struct Profile p8 = {"P8", 10, 15, false};

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

	while (!selectPressed) {

		//resets screen
		ssd1306_FillRectangle(0,0,127,63, Black);

		SRE_Display_Title_Bar("Start Charging");

		int currentScreen = selectedButton/3;
		int navStartIndex = numOfProfiles;
		int navLastIndex = numOfProfiles +2;

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
			sprintf(profileString, "%s: %dA %dV BAL %s", profiles[i].name, profiles[i].current, profiles[i].voltage, profiles[i].isBalancing ? "ON" : "OFF");
			ssd1306_SetCursor(3, y1);

			if (selectedButton == i) {
				ssd1306_FillRectangle(1,y2, 122, y3, White);
				ssd1306_WriteString(profileString, Font_6x8, Black);
			}
			else {
				ssd1306_DrawRectangle(1,y2, 122, y3, White);
				ssd1306_WriteString(profileString, Font_6x8, White);
			}

			y1 = y1 + 12;
			y2 = y2 + 12;
			y3 = y3 + 12;
		}

		//numOfProiles + 2 ensures it will always round up
		int numOfScreens = (numOfProfiles+2)/3;

		SRE_Display_Short_Scroll_Bar(currentScreen, numOfScreens);

		char *navBarButtons[] = {"Batt", "Nav", "Start"};
		SRE_Display_Nav_Bar(navBarButtons,3, navStartIndex);



		ssd1306_UpdateScreen();

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

void SRE_Display_Charger_Stats() {

	while (!selectPressed) {

		ssd1306_FillRectangle(0, 0, 127, 63, Black);


		int numOfButtons = 2;
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
		selectPressed = false;
		if (selectedButton == 0 ) {
			// go to Batt sreen
		}
		else if (selectedButton == 1) {
			//go to Navigation screen
		}
	}
}

void SRE_Display_Battery1(){
	char battery1Title[] = "Battery 1";
	char temperatureStats[] = "Tmp H/L:100.22/50.11C";
	char voltageStats[] = "Vlt H/L:50.11/20.11V";
	char averageStats[] = "Avg T/V:50.22C/20.11V";
	char navButtonText[] = "Nav";
	char battery2ButtonText[] = "Battery 2";

	while(!selectPressed){

		if (selectedButton > 1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = 1;
		}

			ssd1306_SetCursor(1, 2);
			ssd1306_WriteString(battery1Title, Font_6x8, White);
			//x1, y1, x2, y2
				//Not sure if this makes a straight white line as no Cursor isn't selected.
			//call setcursor function?
			ssd1306_Line(0, 10, 127, 10, White);
			//Everything appears to be incremented by Y = 10, so font is roughly 8 squares.
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
			if(selectedButton == 0 || selectedButton > 1){

				ssd1306_SetCursor(3, 54);
				//x1,y1,x2,y2
				ssd1306_FillRectangle(1, 52, 24, 63, White);

				//Has to be last so doesn't get filled.
				ssd1306_WriteString(navButtonText, Font_6x8, Black);
			}else{
				ssd1306_SetCursor(3, 54);
				ssd1306_WriteString(navButtonText, Font_6x8, White);
				ssd1306_DrawRectangle(1, 52, 24, 63, White);
			}

			//Add 2 to x1 and x2 for spacing.
			//Writes button for Battery 2 and selects
			if(selectedButton == 1 || selectedButton < 0){
				//Add 2px for padding for left.
				ssd1306_SetCursor(28, 54);
				ssd1306_FillRectangle(26, 52, 86, 63, White);

				//Has to be last so doesn't get filled.
				ssd1306_WriteString(battery2ButtonText, Font_6x8, Black);

			}else{
				//Add 2px for padding for left.
				ssd1306_SetCursor(28, 54);
				ssd1306_WriteString(battery2ButtonText, Font_6x8, White);
				ssd1306_DrawRectangle(26, 52, 86, 63, White);
			}

			ssd1306_UpdateScreen();
	}



}


void SRE_Display_Battery2(){
	char battery2Title[] = "Battery 2";
	char socStats[] = "SOC: 95.1%";
	char balancingOnOff[] = "Balancing On";

	//Below are vertices for the triangle image.
		//Given text starts at y = 33; and is roughly 8px;
		//Write string goes from top to bottom pixel.
	//Goes from 1-10 x, and is 33-40 high;
	uint8_t x1 = 1, y1 = 40;  // Vertex 1
	uint8_t x2 = 5, y2 = 33;  // Vertex 2
	uint8_t x3 = 10, y3 = 40;  // Vertex 3

	char balancingStats[] = "Balancing: 20.22V";
	char navButtonText[] = "Nav";
	char battery1ButtonText[] = "Battery 1";

	while(!selectPressed){
		if (selectedButton > 1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = 1;
		}
			ssd1306_SetCursor(1, 2);
			ssd1306_WriteString(battery2Title, Font_6x8, White);
			//x1, y1, x2, y2
				//Not sure if this makes a straight white line as no Cursor isn't selected.
			//call setcursor function?
			ssd1306_Line(0, 10, 127, 10, White);
			//Everything appears to be incremented by Y = 10, so font is roughly 8 squares.

			ssd1306_SetCursor(1, 13);
			ssd1306_WriteString(socStats, Font_6x8, White);

			ssd1306_SetCursor(1, 23);
			ssd1306_WriteString(balancingOnOff, Font_6x8, White);

			// Draw the triangle edges
			ssd1306_Line(x1, y1, x2, y2, White);  // Line from Vertex 1 to Vertex 2
			ssd1306_Line(x2, y2, x3, y3, White);  // Line from Vertex 2 to Vertex 3
			ssd1306_Line(x3, y3, x1, y1, White);  // Line from Vertex 3 to Vertex 1

			ssd1306_SetCursor(15, 33);
			ssd1306_WriteString(balancingStats, Font_6x8, White);

			//Writes button for Nav and selects.
			if(selectedButton == 0 || selectedButton > 1){
				ssd1306_SetCursor(3, 54);
				//x1,y1,x2,y2
				ssd1306_FillRectangle(1, 52, 24, 63, White);
				//Has to be last so doesn't get filled.
				ssd1306_WriteString(navButtonText, Font_6x8, Black);
			}else{
				ssd1306_SetCursor(3, 54);
				ssd1306_WriteString(navButtonText, Font_6x8, White);
				ssd1306_DrawRectangle(1, 52, 24, 63, White);
			}
			//Add 2 to x1 and x2 for spacing.
			//Writes button for Battery 2 and selects
			if(selectedButton == 1 || selectedButton < 0){
				//Add 2px for padding for left.
				ssd1306_SetCursor(28, 54);
				ssd1306_FillRectangle(26, 52, 86, 63, White);
				//Has to be last so doesn't get filled.
				ssd1306_WriteString(battery1ButtonText, Font_6x8, Black);
			}else{
				//Add 2px for padding for left.
				ssd1306_SetCursor(28, 54);
				ssd1306_WriteString(battery1ButtonText, Font_6x8, White);
				ssd1306_DrawRectangle(26, 52, 86, 63, White);
			}
			ssd1306_UpdateScreen();
	}

}

//Display Start Balancing
void SRE_Display_StartBalancing(){
	char balancingTitle[] = "Balancing";
	char balancingOnOff[] = "Balancing is off";

	char battButtonText[] = "Batt";
	char navButtonText[] = "Nav";
	char switchBalText[] = "Start Bal";

	//NOTE: Parameters of drawLine and rectangle may be off. Might need to set Cursor
		//for them also before calling them.
		//Can't really test without working OLED.
	//Writes "Charging 1"
		//Change to (1,2) probably
	while(!selectPressed){
		if (selectedButton > 2) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = 1;
		}
			ssd1306_SetCursor(1, 2);
			ssd1306_WriteString(balancingTitle, Font_6x8, White);
			//x1, y1, x2, y2

			//Makes a straight line separating title and text.
			ssd1306_Line(0, 10, 127, 10, White);

			ssd1306_SetCursor(1, 13);
			ssd1306_WriteString(balancingOnOff, Font_6x8, White);

			//3px spacing between buttons.
			//2px between box and text.
			//Writes button for Batt.
			ssd1306_SetCursor(3, 54);
			ssd1306_WriteString(battButtonText, Font_6x8, White);
			ssd1306_DrawRectangle(1, 52, 27, 63, White);

			//nav button
			ssd1306_SetCursor(32, 54);
			ssd1306_WriteString(navButtonText, Font_6x8, White);
			ssd1306_DrawRectangle(30, 52, 51, 63, White);

			//Switch button
			ssd1306_SetCursor(56, 54);
			ssd1306_WriteString(switchBalText, Font_6x8, White);
			ssd1306_DrawRectangle(54, 52, 110, 63, White);
    
			ssd1306_UpdateScreen();
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
	char charging1Title[] = "Charging 1";
	char temperatureStats[] = "Tmp H/L:100.22/50.11C";
	char voltageStats[] = "Vlt H/L:115.97/98.77V";
	char averageStats[] = "Avg T/V:120.11C/5.1V";
	char power[] = "P1: 10A 400V BAL ON";
	char nextButtonText[] = "Next";

	//NOTE: Parameters of drawLine and rectangle may be off. Might need to set Cursor
		//for them also before calling them.
		//Can't really test without working OLED.

	//Writes "Charging 1"
	ssd1306_SetCursor(1, 1);
	ssd1306_WriteString(charging1Title, Font_6x8, White);

	//x1, y1, x2, y2
		//Not sure if this makes a straight white line as no Cursor isn't selected.

	//call setcursor function?
	ssd1306_Line(0, 10, 127, 10, White);

	//Everything appears to be incremented by Y = 10, so font is roughly 8 squares.
	//Writes temp
	ssd1306_SetCursor(1, 13);
	ssd1306_WriteString(temperatureStats, Font_6x8, White);

	//Writes voltage
	ssd1306_SetCursor(1, 23);
	ssd1306_WriteString(voltageStats, Font_6x8, White);

	//Writes averageStats
	ssd1306_SetCursor(1, 33);
	ssd1306_WriteString(averageStats, Font_6x8, White);

	//Writes power
	ssd1306_SetCursor(1, 43);
	ssd1306_WriteString(power, Font_6x8, White);


	//Writes next button
		//Increase in 2 x and 1 y because box takes extra space.
	ssd1306_SetCursor(3, 54);
	ssd1306_WriteString(nextButtonText, Font_6x8, White);
		//draws rectangle surrounding next text
		//x1, y1, x2, y2: from x-1 and y-52, to x-27. Definitely wrong.

	//Called SetCursor for draw rectangle around.
	ssd1306_DrawRectangle(1, 52, 27, 63, White);

	ssd1306_UpdateScreen();


}
