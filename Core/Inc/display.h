// Written by Ayman Alamayri in Dec 2024
#ifndef DISPLAY_H
#define DISPLAY_H

#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "graphics.h"

typedef struct  {
	char name[5];
	uint16_t current;
	uint16_t voltage;
} profile;


typedef enum {
	DISPLAY_STATE_NAVIGATION,
	DISPLAY_STATE_HOME,
	DISPLAY_STATE_CHARGING_PROFILES,
	DISPLAY_STATE_CHARGING_INITIALIZATION,
	DISPLAY_STATE_START_BALANCING,
	DISPLAY_STATE_BALANCING_INITIALIZATION,
	DISPLAY_STATE_BATTERY_STATS_ONE,
	DISPLAY_STATE_BATTERY_STATS_TWO,
	DISPLAY_STATE_CHARGER_STATS,
	DISPLAY_STATE_IN_CHARGING_STATS_ONE,
	DISPLAY_STATE_IN_CHARGING_STATS_TWO,
	DISPLAY_STATE_ERRORS
} displayState;

extern displayState nextDisplayState;
extern displayState currentDisplayState;

void DISP_KanoaSplash();

void Display_init();
displayState Display_updateState();
displayState Display_displayNavigation();
displayState Display_displayHome();
displayState Display_displayErrors();
displayState Display_displayStartBalancing();
displayState Display_displayInChargingStatsOne();
displayState Display_displayInChargingStatsTwo();
displayState Display_displayChargingProfiles();
displayState Display_displayChargerStats();
displayState Display_displayBatteryStatsOne();
displayState Display_displayBatteryStatsTwo();
displayState Display_displayChargingInitialization();
displayState Display_displayBalancingInitialization();
void Display_drawNavBar(char *options[], int numberOfNavBarOptions, int firstNavBarOptionIndex);
void Display_drawTitleBar(char title[]);
void Display_drawInChargingSymbol(int x, int y);
void Display_drawErrorSymbol(int x, int y);
void Display_checkSelectedOptionBounds(int numberOfOptions);
void Display_wrapSelectedOption(int numberOfOptions);
void Display_drawShortScrollBar(int currentView, int numberOfViews);
void Display_drawLongScrollBar(int currentView, int numberOfViews);
void Display_clear();
void Display_forceI2CReset();
void Display_updateScreen();



#endif
