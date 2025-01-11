// Written by Ayman Alamayri in Dec 2024
#ifndef DISPLAY_H
#define DISPLAY_H

void SRE_Display_Init(bool test_mode);
void SRE_Display_Test();
void SRE_Display_Nav();
void SRE_Display_Title_Bar(char title[]);
void SRE_Display_Start_Charging();
void SRE_Display_Nav_Bar(int battNumber, int navNumber, int maxSelectedButton);



#endif
