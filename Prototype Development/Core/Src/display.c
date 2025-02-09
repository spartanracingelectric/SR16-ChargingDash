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

	while (!selectedPress) {
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

	if (selectedPress) {
		selectedPress = false;

		if (selectedButton >= 1 || selectedButton < 0) {
			selectedButton = 0;
		}

		// Goes to some page after abort
	}
}
