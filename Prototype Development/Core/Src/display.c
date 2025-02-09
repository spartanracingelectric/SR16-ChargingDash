// Written by Ayman Alamayri in Dec 2024
#include <stdbool.h>
#include "display.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"

extern int selectedButton;
extern bool selectedPress;

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
	selectedButton = 0;

	char nav[] = "Navigation";
	char home[] = "Home";
	char charge[] = "Start Charging";
	char balance[] = "Start Balancing";
	char battery[] = "Battery";

	char retval;

	// 4 buttons: home, charging, balancing, battery
	while(!selectedPress) {
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

	if (selectedPress) {
		selectedPress = false;

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
