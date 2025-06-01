// Written by Ayman Alamayri in Dec 2024
#include <string.h>
#include <stdbool.h>

#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "graphics.h"

#ifndef DISPLAY_H
#define DISPLAY_H

void DISP_KanoaSplash();

void SRE_Display_Init(bool test_mode);
void SRE_Display_Test();
void SRE_Display_Nav();
void SRE_Display_Charging_Instructions();
void SRE_Display_Home();
void SRE_Display_Nav_Bar(char *buttons[], int numOfButtons, int firstButtonIndex);
void SRE_Display_Err();
void SRE_Display_Start_Balancing();
void SRE_Display_Charging1();
void SRE_Display_Battery2();
void SRE_Display_Charging2();
void SRE_Display_Start_Charging();
void SRE_Display_Short_Scroll_Bar(int currentScreen, int numOfScreens);
void SRE_Display_Charger_Stats();
void SRE_Display_Battery1();
void SRE_Display_Title_Bar(char title[]);
void SRE_Display_Charger_Symbol(int x, int y);
void SRE_Display_Error_Symbol(int x, int y);
void SRE_Display_Long_Scroll_Bar(int currentScreen, int numOfScreens);

#endif
