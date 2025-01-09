// Written by Ayman Alamayri in Dec 2024
#include <stdbool.h>
#include "display.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
extern int selectedButton;
extern bool selectPressed;

// Initialization function
void SRE_Display_Init(bool test_mode) {
	ssd1306_Init();
	if (test_mode) {
		//SRE_Display_Test();
		SRE_Display_Nav();
	}
}

// Function to test display
void SRE_Display_Test() {
	ssd1306_Fill(White);
	ssd1306_UpdateScreen();
}

// Example function to display navigation
void SRE_Display_Nav() {
	selectedButton = 0;
	while(!selectPressed) {

		//only two buttons on this screen so this ensures that the selectedButton doesn't go out of range
		//if the selectedButton reaches the end, resets to the beginning and vice versa
		if (selectedButton > 1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = 1;
		}

		ssd1306_FillRectangle(0,0,127,63, Black);

		ssd1306_SetCursor(1,1);
		ssd1306_WriteString("Navigation", Font_6x8, White);
		ssd1306_Line(0,10,127,10, White);

		if (selectedButton == 0 || selectedButton > 1) {
			ssd1306_FillRectangle(1, 12, 126, 22, White);
			ssd1306_SetCursor(3, 14);
			ssd1306_WriteString("Home", Font_6x8, Black);
		}
		else {
			ssd1306_DrawRectangle(1, 12, 126, 22, White);
			ssd1306_SetCursor(3, 14);
			ssd1306_WriteString("Home", Font_6x8, White);
		}
		if (selectedButton == 1 || selectedButton < 0) {
			ssd1306_FillRectangle(1, 24, 126, 34, White);
			ssd1306_SetCursor(3, 26);
			ssd1306_WriteString("Battery", Font_6x8, Black);

		}
		else {
			ssd1306_DrawRectangle(1, 24, 126, 34, White);
			ssd1306_SetCursor(3, 26);
			ssd1306_WriteString("Battery", Font_6x8, White);
		}


		ssd1306_UpdateScreen();
	}

	if (selectPressed) {
		selectPressed = false;

		//ensures selectedButton is a valid number
		if (selectedButton > 1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = 1;
		}


		if (selectedButton == 0) {
			SRE_Display_Home();
		}
		else if (selectedButton ==1) {
			SRE_Display_Batt();
		}
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

void SRE_Display_Home() {
	selectedButton = 0;
	while (!selectPressed) {

		//only two buttons on this screen so this ensures that the selectedButton doesn't go out of range
		//if the selectedButton reaches the end, it resets to the beginning selectedButton, and vice versa
		if (selectedButton > 1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = 1;
		}

		ssd1306_FillRectangle(0,0,127,63, Black);



		ssd1306_SetCursor(1,1);
		ssd1306_WriteString("Home", Font_6x8, White);
		ssd1306_Line(0,10,127,10, White);

		ssd1306_SetCursor(1,12);
		ssd1306_WriteString("SOC: 85.7%", Font_6x8, White);
		SRE_Display_Nav_Bar(0, 1, 1);




		ssd1306_UpdateScreen();

	}
	if (selectPressed) {
		selectPressed = false;

		//ensures selectedButton is a valid number
		if (selectedButton > 1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = 1;
		}


		if (selectedButton == 0) {
			SRE_Display_Batt();
		}
		else if (selectedButton == 1) {
			SRE_Display_Nav();
		}
	}
}

void SRE_Display_Batt() {
	selectedButton = 0;
	while (!selectPressed) {

		if (selectedButton > 0) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = 0;
		}

		ssd1306_FillRectangle(0,0,127,63, Black);



		ssd1306_SetCursor(1,1);
		ssd1306_WriteString("Battery 1", Font_6x8, White);
		ssd1306_Line(0,10,127,10, White);

		ssd1306_SetCursor(1,12);
		ssd1306_WriteString("Tmp H/L:100.22/50.11C", Font_6x8, White);


		//there's only one button so its always going to be highlighted
		if (selectedButton == 0 || selectedButton !=0 ) {
			ssd1306_FillRectangle(1, 52, 21, 62, White);
			ssd1306_SetCursor(3, 54);
			ssd1306_WriteString("Nav", Font_6x8, Black);
		}
		else {
			ssd1306_DrawRectangle(1, 52, 21, 62, White);
			ssd1306_SetCursor(3, 54);
			ssd1306_WriteString("Nav", Font_6x8, White);
		}

		ssd1306_UpdateScreen();

	}
	if (selectPressed) {
		selectPressed = false;

		if (selectedButton > 0) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = 0;
		}

		if (selectedButton == 0) {
			SRE_Display_Nav();
		}
	}
}

