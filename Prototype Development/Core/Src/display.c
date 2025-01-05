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

// Example function to display charging 1
void SRE_Display_Charging1() {
	char charging1Title[] = "Charging 1";
	char temperatureStats[] = "Tmp H/L:100.22/50.11C";
	char voltageStats[] = "Vlt H/L:115.97/98.77V";
	char averageStats[] = "Avg T/V:120.11C/5.1V";
	char power[] = "P1: 10A 400V BAL ON";
	char nextButtonText[] = "Next";

	//Starts up the OLED basically.
	//Replace with SRE_Display_Init later?
	ssd1306_Init();



	//NOTE: Parameters of drawLine and rectangle may be off. Might need to set Cursor
		//for them also before calling them.
		//Can't really test without working OLED.






	//Writes "Charging 1"
	ssd1306_SetCursor(1, 1);
	ssd1306_WriteString(charging1Title, Font_6x8, White);

	//x1, y1, x2, y2
		//Not sure if this makes a straight white line as no Cursor isn't selected.

	//call setcursor function?
	ssd1306_Line(0, 10, 127, 10, White);

	//Everything appears to be incremented by Y = 10, so font is roughly 8 squares.
	//Writes temp
	ssd1306_SetCursor(1, 13);
	ssd1306_WriteString(temperatureStats, Font_6x8, White);

	//Writes voltage
	ssd1306_SetCursor(1, 23);
	ssd1306_WriteString(voltageStats, Font_6x8, White);

	//Writes averageStats
	ssd1306_SetCursor(1, 33);
	ssd1306_WriteString(averageStats, Font_6x8, White);

	//Writes power
	ssd1306_SetCursor(1, 43);
	ssd1306_WriteString(power, Font_6x8, White);


	//Writes next button
		//Increase in 2 x and 1 y because box takes extra space.
	ssd1306_SetCursor(3, 54);
	ssd1306_WriteString(power, Font_6x8, White);
		//draws rectangle surrounding next text
		//x1, y1, x2, y2: from x-1 and y-52, to x-27. Definitely wrong.

	//Called SetCursor for draw rectangle around.
	ssd1306_DrawRectangle(1, 52, 27, 11, White);



}
