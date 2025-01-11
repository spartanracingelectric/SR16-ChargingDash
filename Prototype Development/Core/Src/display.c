// Written by Ayman Alamayri in Dec 2024
#include <stdbool.h>
#include "display.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"

// Initialization function
void SRE_Display_Init(bool test_mode) {
	ssd1306_Init();
	if (test_mode) {
		SRE_Display_Test();
		//SRE_Display_Start_Charging();
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


	ssd1306_UpdateScreen();

}

void SRE_Display_Start_Charging() {
	struct Profile {
		char profileName[5];
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

	struct Profile profiles[6];
	profiles[0] = p1;
	profiles[1] = p2;
	profiles[2] = p3;
	profiles[3] = p4;
	profiles[4] = p5;
	profiles[5] = p6;

 	SRE_Display_Title_Bar("Start Charging");

	ssd1306_SetCursor(3, 15);
	ssd1306_WriteString("P1: 10A 20V BAL ON", Font_6x8, White);
	ssd1306_DrawRectangle(1,13, 122, 23, White);

	ssd1306_SetCursor(3, 27);
	ssd1306_WriteString("P2: 10A 400V BAL OFF", Font_6x8, White);
	ssd1306_DrawRectangle(1,25, 122, 35, White);

	ssd1306_SetCursor(3, 39);
	ssd1306_WriteString("P3: 10A 400V BAL OFF", Font_6x8, White);
	ssd1306_DrawRectangle(1,37, 122, 47, White);

	ssd1306_DrawRectangle(124, 13, 126, 47, White);
	ssd1306_Line(125, 30, 125, 46, White);

	SRE_Display_Nav_Bar(0,1, 1);
	ssd1306_SetCursor(53, 54);
	ssd1306_WriteString("Start", Font_6x8, White);
	ssd1306_DrawRectangle(51, 52, 83, 62, White);


ssd1306_UpdateScreen();


}

void SRE_Display_Nav_Bar(int battNumber, int navNumber, int maxSelectedButton) {

	//the selectedButton values for nav bar will vary based on currenty displayed screen
	int selectedButton = 0;

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




	ssd1306_UpdateScreen();


}
