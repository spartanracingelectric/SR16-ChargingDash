// Written by Ayman Alamayri in Dec 2024
#include <stdbool.h>
#include "display.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"

// Initialization function
void SRE_Display_Init(bool test_mode) {
	ssd1306_Init();
	if (test_mode) {
		SRE_Display_Charging2();
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

void SRE_Display_Charging2(){
		char charging2Title[] = "Charging 2";
		char packVoltStats[] = "Pack Volt: 400.22V";
		char socStats[] = "SOC: 92.7%";
		char timeRemaining[] = "Time Remaining: 120m";

		char charging1Button[] = "Charging 1";

		//Starts up the OLED basically.
		//Replace with SRE_Display_Init later?
		ssd1306_Init();



		//NOTE: Parameters of drawLine and rectangle may be off. Might need to set Cursor
			//for them also before calling them.
			//Can't really test without working OLED.






		//Writes "Charging 1"
		ssd1306_SetCursor(1, 1);
		ssd1306_WriteString(charging2Title, Font_6x8, White);

		//x1, y1, x2, y2
			//Not sure if this makes a straight white line as no Cursor isn't selected.


		ssd1306_Line(0, 10, 127, 10, White);

		//Everything appears to be incremented by Y = 10, so font is roughly 8 squares.
		//Writes Pack Volt Stats
		ssd1306_SetCursor(1, 13);
		ssd1306_WriteString(packVoltStats, Font_6x8, White);

		//Writes SOC Stats
		ssd1306_SetCursor(1, 23);
		ssd1306_WriteString(socStats, Font_6x8, White);

		//Writes timeRemaining
		ssd1306_SetCursor(1, 33);
		ssd1306_WriteString(timeRemaining, Font_6x8, White);


		//Writes charging1 button
			//Increase in 2 x and 1 y because box takes extra space.
		ssd1306_SetCursor(3, 54);
		ssd1306_WriteString(charging1Button, Font_6x8, White);
			//draws rectangle surrounding next text
			//x1, y1, x2, y2:

		ssd1306_DrawRectangle(1, 52, 64, 63, White);

		ssd1306_UpdateScreen();

}
