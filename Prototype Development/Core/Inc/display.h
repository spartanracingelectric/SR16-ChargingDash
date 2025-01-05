// Written by Ayman Alamayri in Dec 2024
#ifndef DISPLAY_H
#define DISPLAY_H

void SRE_Display_Init(bool test_mode);
void SRE_Display_Test();
void SRE_Display_Nav();
void SRE_Display_Title_Bar(char title[]);
void SRE_Display_Charger_Symbol(int x, int y);
void SRE_Display_Error_Symbol(int x, int y);

#endif
