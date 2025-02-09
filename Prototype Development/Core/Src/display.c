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

}

void SRE_Display_Err() {
	selectedButton = 0;

	char err[] = "Errors";
	char err1[] = "Error message 1";
	char err2[] = "Error message 2";
	char err3[] = "Error message 3";

	char retval;

	while (!selectedPress) {
		ssd1306_FillRectangle(0, 0, 127, 63, Black);

		if (selectedButton > 1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = 1;
		}

		ssd1306_SetCursor(1, 1);
		retval = ssd1306_WriteString(err, Font_16x15, White);
		ssd1306_Line(0, 10, 127, 10, White);

		ssd1306_SetCursor(3, 15);
		retval = ssd1306_WriteString(err1, Font_16x15, White);
		ssd1306_DrawRectangle(1, 13, 122, 11, White);

		ssd1306_SetCursor(3, 28);
		retval = ssd1306_WriteString(err2, Font_16x15, White);
		ssd1306_DrawRectangle(1, 26, 122, 11, White);

		ssd1306_SetCursor(3, 41);
		retval = ssd1306_WriteString(err3, Font_16x15, White);
		ssd1306_DrawRectangle(1, 39, 122, 11, White);

		char *navBarButtons[] = {"Batt", "Nav"};

		SRE_Display_Nav_Bar(navBarButtons, 2, 0);

		ssd1306_UpdateScreen();
	}

	if (selectedPress) {
		selectedPress = false;

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
