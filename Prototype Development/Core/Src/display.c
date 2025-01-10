// Written by Ayman Alamayri in Dec 2024
#include <stdbool.h>
#include "display.h"
#include "ssd1306.h"

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
	// Error title
	char err[] = "Errors";
	char retval;

	ssd1306_SetCursor(1, 1);
	retval = ssd1306_WriteString(err, Font_16x15, White);

	// Draw line
	ssd1306_Line(0, 10, 127, 10, White);

	// Sample error msg
	char err1[] = "Error message 1";
	char retval;

	ssd1306_SetCursor(3, 15);
	retval = ssd1306_WriteString(err1, Font_16x15, White);

	// Draw Rect
	ssd1306_DrawRectangle(1, 13, 122, 11, White);

	// Sample error 2 msg
	char err2[] = "Error message 2";
	char retval;

	ssd1306_SetCursor(3, 28);
	retval = ssd1306_WriteString(err2, Font_16x15, White);

	// Draw Rect
	ssd1306_DrawRectangle(1, 26, 122, 11, White);

	// Sample error 3 msg
	char err3[] = "Error message 3";
	char retval;

	ssd1306_SetCursor(3, 41);
	retval = ssd1306_WriteString(err3, Font_16x15, White);

	// Draw Rect
	ssd1306_DrawRectangle(1, 39, 122, 11, White);

	ssd1306_UpdateScreen();
}
