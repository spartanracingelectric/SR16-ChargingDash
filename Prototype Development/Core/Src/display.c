// Written by Ayman Alamayri in Dec 2024
#include <stdbool.h>
#include "display.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define SCROLLBAR_WIDTH 2
#define SCROLLBAR_X_POS (OLED_WIDTH - SCROLLBAR_WIDTH)
#define SCROLLBAR_SEGMENT_HEIGHT 4
#define SCROLLBAR_SPACING 2

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

// Function to display navigation
void SRE_Display_Nav() {
	//Navigation title
	char nav[] = "Navigation";
	char retval;

	ssd1306_SetCursor(1, 1);
	retval = ssd1306_WriteString(nav, Font_16x15, White);

	// Draw line
	ssd1306_Line(0, 10, 127, 10, White);

	// Home
	char home[] = "Home";

	ssd1306_SetCursor(3, 15);
	retval = ssd1306_WriteString(home, Font_16x15, White);

	// Draw rect
	ssd1306_DrawRectangle(1, 13, 122, 11, White);

	// Charging
	char charge[] = "Start Charging";

	ssd1306_SetCursor(3, 28);
	retval = ssd1306_WriteString(charge, Font_16x15, White);

	// Draw rect
	ssd1306_DrawRectangle(1, 26, 122, 11, White);

	// Balancing
	char balance[] = "Start Balancing";

	ssd1306_SetCursor(3, 41);
	retval = ssd1306_WriteString(balance, Font_16x15, White);

	// Draw rect
	ssd1306_DrawRectangle(1, 39, 122, 11, White);

	// Balancing
	char battery[] = "Battery";

	ssd1306_SetCursor(3, 54);
	retval = ssd1306_WriteString(battery, Font_16x15, White);

	// Draw rect
	ssd1306_DrawRectangle(1, 52, 122, 11, White);

	//will add scrollbar once I can test

	ssd1306_UpdateScreen();
}

// Function to draw scrollbar
void draw_scrollbar(uint8_t content_height, uint8_t viewport_height, uint8_t scroll_pos) {

	// Everything fits on screen
	if (viewport_height >= content_height) {
		return;
	}

	uint8_t total_height = OLED_HEIGHT;
	uint8_t scrollable_area = total_height - (SCROLLBAR_SEGMENT_HEIGHT + SCROLLBAR_SPACING);
	uint8_t visible_area = (viewport_height * scrollable_area) / content_height;
	uint8_t scroll_position_area = (scroll_pos * (scrollable_area - visible_area)) / (content_height - viewport_height);

	// Clear previous scrollbar area
	ssd1306_FillRectangle(SCROLLBAR_X_POS, 0, SCROLLBAR_WIDTH, OLED_HEIGHT, Black);

	// Draw new scrollbar area
	for (uint8_t i = 0; i < visible_area; i += SCROLLBAR_SEGMENT_HEIGHT + SCROLLBAR_SPACING) {
		uint8_t y_pos = scroll_position_area + y;

		if (y_pos + SCROLLBAR_SEGMENT_HEIGHT > total_height) {
			break;
		}
		ssd1306_FillRectangle(SCROLLBAR_X_POS, y_pos, SCROLLBAR_WIDTH, SCROLLBAR_SEGMENT_HEIGHT, White);
	}

	ssd1306_UpdateScreen();
}
