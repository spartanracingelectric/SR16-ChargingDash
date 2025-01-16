// Written by Ayman Alamayri in Dec 2024
#include <stdbool.h>
#include "display.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>


// Initialization function
void SRE_Display_Init(bool test_mode) {
	ssd1306_Init();
	if (test_mode) {
		SRE_Display_Test();
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
		int battButtonIndex = numOfProfiles;
		int startButtonIndex = numOfProfiles + 2;
		int navStartIndex = numOfProfiles;

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
			selectedButton = startButtonIndex;
		}
		//going down from start button will reset back to first profile being selected
		if (selectedButton > startButtonIndex) {
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


