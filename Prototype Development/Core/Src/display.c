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
		SRE_Display_Battery2();
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

void SRE_Display_Battery2(){
		char battery2Title[] = "Battery 2";
		char socStats[] = "SOC: 95.1%";
		char balancingOnOff[] = "Balancing On";

		//Below are vertices for the triangle image.
			//Given text starts at y = 33; and is roughly 8px;
			//Write string goes from top to bottom pixel.
		//Goes from 1-10 x, and is 33-40 high;
	    uint8_t x1 = 1, y1 = 40;  // Vertex 1
	    uint8_t x2 = 5, y2 = 33;  // Vertex 2
	    uint8_t x3 = 10, y3 = 40;  // Vertex 3



		char balancingStats[] = "Balancing: 20.22V";
		char navButtonText[] = "Nav";
		char battery2ButtonText[] = "Battery 1";
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
				ssd1306_WriteString(battery2Title, Font_6x8, White);
				//x1, y1, x2, y2
					//Not sure if this makes a straight white line as no Cursor isn't selected.
				//call setcursor function?
				ssd1306_Line(0, 10, 127, 10, White);
				//Everything appears to be incremented by Y = 10, so font is roughly 8 squares.

				ssd1306_SetCursor(1, 13);
				ssd1306_WriteString(socStats, Font_6x8, White);

				ssd1306_SetCursor(1, 23);
				ssd1306_WriteString(balancingOnOff, Font_6x8, White);

			    // Draw the triangle edges
			    ssd1306_Line(x1, y1, x2, y2, White);  // Line from Vertex 1 to Vertex 2
			    ssd1306_Line(x2, y2, x3, y3, White);  // Line from Vertex 2 to Vertex 3
			    ssd1306_Line(x3, y3, x1, y1, White);  // Line from Vertex 3 to Vertex 1

				ssd1306_SetCursor(15, 33);
				ssd1306_WriteString(balancingStats, Font_6x8, White);


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
