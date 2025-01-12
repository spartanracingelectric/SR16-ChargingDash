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
		SRE_Display_Balancing();
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

void SRE_Display_Balancing(){
			char balancingTitle[] = "Balancing";
			char balancingOnOff[] = "Balancing is off";

			char battButtonText[] = "Batt";
			char navButtonText[] = "Nav";
			char switchBalText[] = "Start Bal";



			ssd1306_Init();
			//NOTE: Parameters of drawLine and rectangle may be off. Might need to set Cursor
				//for them also before calling them.
				//Can't really test without working OLED.
			//Writes "Charging 1"
				//Change to (1,2) probably
			while(!selectPressed){
				if (selectedButton > 2) {
					selectedButton = 0;
				}
				if (selectedButton < 0) {
					selectedButton = 1;
				}
					ssd1306_SetCursor(1, 2);
					ssd1306_WriteString(balancingTitle, Font_6x8, White);
					//x1, y1, x2, y2

					//Makes a straight line separating title and text.
					ssd1306_Line(0, 10, 127, 10, White);


					ssd1306_SetCursor(1, 13);
					ssd1306_WriteString(balancingOnOff, Font_6x8, White);


					//3px spacing between buttons.
					//2px between box and text.

					//Writes button for Batt.
					ssd1306_SetCursor(3, 54);
					ssd1306_WriteString(battButtonText, Font_6x8, White);
					ssd1306_DrawRectangle(1, 52, 27, 63, White);


					//nav button
					ssd1306_SetCursor(32, 54);
					ssd1306_WriteString(navButtonText, Font_6x8, White);
					ssd1306_DrawRectangle(30, 52, 51, 63, White);


					//Switch button
					ssd1306_SetCursor(56, 54);
					ssd1306_WriteString(switchBalText, Font_6x8, White);
					ssd1306_DrawRectangle(54, 52, 110, 63, White);




					ssd1306_UpdateScreen();
			}

	}

