// Written by Ayman Alamayri in Dec 2024
#include <stdbool.h>
#include "display.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
extern int selectedButton;

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




	ssd1306_UpdateScreen();


}
