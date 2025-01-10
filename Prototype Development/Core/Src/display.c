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
		//Remember to add function here to test.
//		SRE_Display_Test();
		SRE_Display_Battery1();
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

void SRE_Display_Battery1(){
	char battery1Title[] = "Battery 1";
	char temperatureStats[] = "Tmp H/L:100.22/50.11C";
	char voltageStats[] = "Vlt H/L:50.11/20.11V";
	char averageStats[] = "Avg T/V:50.22C/20.11V";

	char navButtonText[] = "Nav";
	char battery2ButtonText[] = "Battery 2";
	//Starts up the OLED basically.
	//Replace with SRE_Display_Init later?
	ssd1306_Init();
	//NOTE: Parameters of drawLine and rectangle may be off. Might need to set Cursor
		//for them also before calling them.
		//Can't really test without working OLED.
	//Writes "Charging 1"
		//Change to (1,2) probably


	while(!selectPressed){

		if (selectedButton > 1) {
			selectedButton = 0;
		}
		if (selectedButton < 0) {
			selectedButton = 1;
		}

			ssd1306_SetCursor(1, 2);
			ssd1306_WriteString(battery1Title, Font_6x8, White);
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

			//Writes button for Nav and selects.
			if(selectedButton == 0 || selectedButton > 1){

				ssd1306_SetCursor(3, 54);
				//x1,y1,x2,y2
				ssd1306_FillRectangle(1, 52, 24, 63, White);

				//Has to be last so doesn't get filled.
				ssd1306_WriteString(navButtonText, Font_6x8, Black);
			}else{
				ssd1306_SetCursor(3, 54);
				ssd1306_WriteString(navButtonText, Font_6x8, White);
				ssd1306_DrawRectangle(1, 52, 24, 63, White);
			}


			//Add 2 to x1 and x2 for spacing.
			//Writes button for Battery 2 and selects
			if(selectedButton == 1 || selectedButton < 0){
				//Add 2px for padding for left.
				ssd1306_SetCursor(28, 54);
				ssd1306_FillRectangle(26, 52, 86, 63, White);

				//Has to be last so doesn't get filled.
				ssd1306_WriteString(battery2ButtonText, Font_6x8, Black);

			}else{
				//Add 2px for padding for left.
				ssd1306_SetCursor(28, 54);
				ssd1306_WriteString(battery2ButtonText, Font_6x8, White);
				ssd1306_DrawRectangle(26, 52, 86, 63, White);
			}


			ssd1306_UpdateScreen();
	}



}
