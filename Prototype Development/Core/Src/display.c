// Written by Ayman Alamayri in Dec 2024
#include <stdbool.h>
#include "display.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>
extern int selectedButton;
extern bool selectPressed;

// Initialization function
void SRE_Display_Init(bool test_mode) {
	ssd1306_Init();
	if (test_mode) {
		//SRE_Display_Test();
		SRE_Display_Start_Charging();
	}
}

// Function to test display
void SRE_Display_Test() {
	ssd1306_Fill(White);
	ssd1306_UpdateScreen();
}

// Example function to display navigation
void SRE_Display_Nav() {

}

void SRE_Display_Title_Bar(char title[]) {

	ssd1306_SetCursor(1,1);
	ssd1306_WriteString(title, Font_6x8, White);
	ssd1306_Line(0, 10, 127, 10, White);


	//ssd1306_UpdateScreen();

}

void SRE_Display_Start_Charging() {

	//defines an array of Profiles to be used as test data
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
		ssd1306_FillRectangle(0,0,127,63, Black);
		SRE_Display_Title_Bar("Start Charging");


		int currentScreen = selectedButton/3;
		if (selectedButton > numOfProfiles -1) {
			currentScreen = (numOfProfiles-1)/3;
		}
		int startIndex = currentScreen*3;



		if (selectedButton > numOfProfiles-1) {
			startIndex = (numOfProfiles-1)/3*3;
			//startIndex = numOfProfiles-3;
		}
		if (selectedButton < 0) {
			startIndex = 0;
			selectedButton = numOfProfiles+2;
		}
		if (selectedButton > numOfProfiles+2) {
			startIndex = 0;
			selectedButton = 0;
		}

		int y1 = 15;
		int y2 = 13;
		int y3 = 23;

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

		if (numOfScreens > 1) {

			int scrollContainerHeight = 35;
			int scrollBarLength = scrollContainerHeight/numOfScreens;

			int scrollY = 14+ (currentScreen*scrollBarLength);

			ssd1306_DrawRectangle(124, 13, 126, 47, White);
			ssd1306_Line(125, scrollY, 125, scrollY+scrollBarLength, White);
		}


		SRE_Display_Nav_Bar(numOfProfiles,numOfProfiles+1, numOfProfiles+2);
		ssd1306_SetCursor(53, 54);
		if (selectedButton == numOfProfiles+2) {
			ssd1306_FillRectangle(51, 52, 83, 62, White);
			ssd1306_WriteString("Start", Font_6x8, Black);

		}
		else {
			ssd1306_DrawRectangle(51, 52, 83, 62, White);
			ssd1306_WriteString("Start", Font_6x8, White);
		}

		ssd1306_UpdateScreen();

	}
}

void SRE_Display_Nav_Bar(int battNumber, int navNumber, int maxSelectedButton) {

	//the selectedButton values for nav bar will vary based on currenty displayed screen

	if (selectedButton == battNumber || (battNumber == 0 && selectedButton > maxSelectedButton)) {
		ssd1306_FillRectangle(1, 52, 27, 62, White);
		ssd1306_SetCursor(3, 54);
		ssd1306_WriteString("Batt", Font_6x8, Black);
	}
	else {
		ssd1306_DrawRectangle(1, 52, 27, 62, White);
		ssd1306_SetCursor(3, 54);
		ssd1306_WriteString("Batt", Font_6x8, White);
	}

	if (selectedButton == navNumber || (navNumber == maxSelectedButton && selectedButton < 0)) {
		ssd1306_FillRectangle(29, 52, 49, 62, White);
		ssd1306_SetCursor(31, 54);
		ssd1306_WriteString("Nav", Font_6x8, Black);
	}
	else {
		ssd1306_DrawRectangle(29, 52, 49, 62, White);
		ssd1306_SetCursor(31, 54);
		ssd1306_WriteString("Nav", Font_6x8, White);
	}
}
