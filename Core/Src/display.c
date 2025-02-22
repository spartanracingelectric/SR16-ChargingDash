// Written by Ayman Alamayri in Dec 2024
#include <stdbool.h>
#include "display.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <string.h>
extern int selectedButton;
extern bool selectPressed;
extern int backPressed;

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

void SRE_Display_Nav_Bar(char *buttons[], int numOfButtons, int firstButtonIndex) {

	//the selectedButton values for nav bar will vary based on currenty displayed screen
	int maxSelectedButtonIndex = firstButtonIndex + numOfButtons-1;
	int buttonIndex = firstButtonIndex;

	int x1 = 1;
	int x2 = 1;

	for (int i = 0; i < numOfButtons; i++) {
		x2 = x1 + (strlen(buttons[i]) * 6) + 2;
		if (selectedButton == buttonIndex ||
			(buttonIndex == 0 && selectedButton > maxSelectedButtonIndex) ||
			(buttonIndex == maxSelectedButtonIndex && selectedButton < 0))
		{
			ssd1306_FillRectangle(x1, 52, x2, 62, White);
			ssd1306_SetCursor(x1 + 2, 54);
			ssd1306_WriteString(buttons[i], Font_6x8, Black);
		}
		else {
			ssd1306_DrawRectangle(x1, 52, x2, 62, White);
			ssd1306_SetCursor(x1 + 2, 54);
			ssd1306_WriteString(buttons[i], Font_6x8, White);
		}

		buttonIndex++;
		x1 = x2 + 2;
	}


	ssd1306_UpdateScreen();
}

//Display Start Balancing
void SRE_Display_StartBalancing(){
	char balancingTitle[] = "Balancing";
	char balancingOnOff[] = "Balancing is off";

	char battButtonText[] = "Batt";
	char navButtonText[] = "Nav";
	char switchBalText[] = "Start Bal";


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