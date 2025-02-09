// Written by Ayman Alamayri in Dec 2024
#include <stdbool.h>
#include "display.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <string.h>

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


