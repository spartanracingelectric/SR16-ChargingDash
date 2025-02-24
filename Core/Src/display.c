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
	//ssd1306_Fill(White);
	char *navButtons[] = {"Nav", "Batt", "Start"};
	SRE_Display_Nav_Bar(navButtons, 3, 0);
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

void SRE_Display_Battery1(){
	char battery1Title[] = "Battery 1";
	char temperatureStats[] = "Tmp H/L:100.22/50.11C";
	char voltageStats[] = "Vlt H/L:50.11/20.11V";
	char averageStats[] = "Avg T/V:50.22C/20.11V";
	char navButtonText[] = "Nav";
	char battery2ButtonText[] = "Battery 2";

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

void SRE_Display_Title_Bar(char title[]) {

	ssd1306_SetCursor(1,1);
	ssd1306_WriteString(title, Font_6x8, White);
	ssd1306_Line(0, 10, 127, 10, White);

	SRE_Display_Charger_Symbol(88, 3);
	SRE_Display_Error_Symbol(119,1);
}

void SRE_Display_Charger_Symbol(int x, int y) {
	//point of origin (x,y) is the top left of battery
	ssd1306_Line(x, y, x+4, y, White);
	ssd1306_Line(x, y, x, y+4, White);
	ssd1306_Line(x, y+4, x+4, y+4, White);

	ssd1306_Line(x+12, y, x+16, y, White);
	ssd1306_Line(x+16, y, x+16, y+4, White);
	ssd1306_Line(x+12, y+4, x+16, y+4, White);
	ssd1306_Line(x+17, y+1, x+17, y+3, White);

	ssd1306_Line(x+6, y+2, x+10, y+2, White);
	ssd1306_Line(x+6, y+2, x+9, y-1, White);
	ssd1306_Line(x+10, y+2, x+7, y+5, White);
}

void SRE_Display_Error_Symbol(int x, int y) {
	//point of origin (x,y) is the top of the triangle
	ssd1306_Line(x, y, x+7, y+7, White);
	ssd1306_Line(x, y, x-7, y+7, White);
	ssd1306_Line(x-7, y+7, x+7, y+7, White);

	ssd1306_Line(x, y+2, x, y+4, White);
	ssd1306_Line(x, y+6, x, y+6, White);
}


