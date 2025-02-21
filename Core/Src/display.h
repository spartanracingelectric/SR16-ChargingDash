// Written by Ayman Alamayri in Dec 2024
#include <stdbool.h>

#ifndef DISPLAY_H
#define DISPLAY_H
void SRE_Display_Init(bool test_mode);
void SRE_Display_Test();
void SRE_Display_Nav();
void SRE_Display_Nav_Bar(char *buttons[], int numOfButtons, int firstButtonIndex);
#endif
