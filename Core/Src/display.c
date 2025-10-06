// Written by Ayman Alamayri in Dec 2024
#include "display.h"
#include "charger.h"
#include <stdio.h>

profile all_profiles[] = {
	{"P1", 3, 355},
	{"P2", 4, 355},
	{"P3", 20, 355},
	{"P4", 3, 385},
	{"P5", 15, 385},
	{"P6", 20, 385},
	{"P7", 10, 401},
	{"P8", 20, 400},
	{"P9", 10, 403},	
};

int number_of_profiles = 9;

int selected_option = 0;
bool select_pressed = false;
bool back_pressed = false;

display_state current_display_state = DISPLAY_STATE_NAVIGATION;
display_state next_display_state = DISPLAY_STATE_NAVIGATION;


extern char codeBranch[10];
extern char codeVersion[5];

void DISP_KanoaSplash() {
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	ssd1306_SetCursor(64, 15);
	ssd1306_WriteString("Elcon Control", Font_6x8, White);
	ssd1306_SetCursor(64, 25);
	ssd1306_WriteString("By Ayman A., et al", Font_6x8, White);
	ssd1306_SetCursor(64, 35);
	ssd1306_WriteString(codeVersion, Font_6x8, White);
	ssd1306_SetCursor(64, 45);
	ssd1306_WriteString(codeBranch, Font_6x8, White);
	ssd1306_UpdateScreen();
}

// Initialization function
void display_init() {
	ssd1306_Init();
}

display_state display_update_state() {
	if (current_display_state != next_display_state) {
		selected_option = 0;
		select_pressed = false;
		current_display_state = next_display_state;
	}

	switch(current_display_state) {
		case DISPLAY_STATE_NAVIGATION:
			next_display_state = display_navigation(); 
			break;
		case DISPLAY_STATE_HOME:
			next_display_state = display_home();
			break;
		case DISPLAY_STATE_CHARGING_PROFILES:
			next_display_state = display_charging_profiles();
			break;
		case DISPLAY_STATE_CHARGING_INITIALIZATION:
			next_display_state = display_charging_initialization();
			break;
		case DISPLAY_STATE_START_BALANCING:
			next_display_state = display_start_balancing();
			break;
		case DISPLAY_STATE_BALANCING_INITIALIZATION:
			next_display_state = display_balancing_initialization();
			break;
		case DISPLAY_STATE_BATTERY_STATS_ONE:
			next_display_state = display_battery_stats_one();
			break;
		case DISPLAY_STATE_BATTERY_STATS_TWO:
			next_display_state = display_battery_stats_two();
			break;
		case DISPLAY_STATE_CHARGER_STATS:
			next_display_state = display_charger_stats();
			break;
		case DISPLAY_STATE_IN_CHARGING_STATS_ONE:
			next_display_state = display_in_charging_stats_one();
			break;
		case DISPLAY_STATE_IN_CHARGING_STATS_TWO:
			next_display_state = display_in_charging_stats_two();
			break;
		case DISPLAY_STATE_ERRORS:
			next_display_state = display_errors();
			break;
	}
	return next_display_state;
}

void display_clear() {
	ssd1306_FillRectangle(0, 0, 127, 63, Black);
}

void display_wrap_selected_option(int number_of_options) {
	if (selected_option > number_of_options - 1 ){
		selected_option = 0;
	}
	if (selected_option< 0) {
		selected_option = number_of_options - 1;
	}
}

void display_check_selected_option_bounds(int number_of_options) {
	if (selected_option > number_of_options - 1) {
		selected_option = 0;
	}
	if (selected_option < 0) {
		selected_option = number_of_options;
	}
}

display_state display_navigation() {
	static char* options[] = {"Home", "Charging", "Balancing", "Battery", "Charger Stats", "Errors", "Restart"};
	int number_of_options = 7;
	int y1 = 15;
	int y2 = 13;
	int y3 = 24;
	int current_view = selected_option/4;
	int start_index = current_view*4;

	display_clear();
	display_wrap_selected_option(number_of_options);

	display_draw_title_bar("Navigation");

	for (int i = start_index; i < start_index + 4 && i < selected_option; i++) {
		ssd1306_SetCursor(3, y1);
		if (selected_option == i) {
			ssd1306_FillRectangle(1,y2, 122, y3, White);
			ssd1306_WriteString(options[i], Font_6x8, Black);
		}
		else {
			ssd1306_DrawRectangle(1,y2, 122, y3, White);
			ssd1306_WriteString(options[i], Font_6x8, White);
		}

		y1 = y1 + 13;
		y2 = y2 + 13;
		y3 = y2 + 10;
	}

	int number_of_views = (number_of_options +3 ) / 4; //3 options per view, rounds up to ensure there is enough views
	display_draw_long_scroll_bar(current_view, number_of_views);

	ssd1306_UpdateScreen();
	
	if (select_pressed) {
		check_selected_option_bounds(number_of_options);
		switch(selected_option) {
			case 0: return DISPLAY_STATE_HOME;
			case 1: return DISPLAY_STATE_CHARGING_PROFILES;
			case 2: return DISPLAY_STATE_START_BALANCING;
			case 3: return DISPLAY_STATE_BATTERY_STATS_ONE;
			case 4: return DISPLAY_STATE_BATTERY_STATS_TWO;
			case 5: return DISPLAY_STATE_CHARGER_STATS;
			case 6: return DISPLAY_STATE_ERRORS;
			case 7: NVIC_SystemReset();
		}
	}
	return DISPLAY_STATE_NAVIGATION;
}


display_state display_home() {
	char state_of_charge[50];
	char balancing_status[50];

	//TODO READ CHARGER TEMP
	char charger_temp[] = "Charger Tmp: 100.1C";
	int number_of_options = 1;

	display_clear();
	display_wrap_selected_option(number_of_options);


	display_draw_title_bar("Home");

	ssd1306_SetCursor(1, 13);
	ssd1306_WriteString(state_of_charge, Font_6x8, White);

	ssd1306_SetCursor(1, 23);
	ssd1306_WriteString(charger_temp, Font_6x8, White);

	ssd1306_SetCursor(1, 33);
	ssd1306_WriteString(balancing_status, Font_6x8, White);

	char *nav_bar_options[] = {"Nav"};
	int first_option_index = 0;
	display_draw_nav_bar(nav_bar_options, number_of_options, first_option_index);

	ssd1306_UpdateScreen();

	if (select_pressed) {
		display_check_selected_option_bounds(number_of_options);
		switch(selected_option) {
			case 0: return DISPLAY_STATE_NAVIGATION;
			case 1: return DISPLAY_STATE_BATTERY_STATS_ONE;
		}
	}
	return DISPLAY_STATE_HOME;
}

void display_draw_nav_bar(char *options[], int number_of_nav_bar_options, int first_nav_bar_option_index) {

	int nav_bar_option_index = first_nav_bar_option_index;

	int x1 = 1;
	int x2 = 1;

	for (int i = 0; i < number_of_nav_bar_options; i++) {
		x2 = x1 + (strlen(options[i]) * 6) + 2;
		if (selected_option == nav_bar_option_index) {
			ssd1306_FillRectangle(x1, 52, x2, 62, White);
			ssd1306_SetCursor(x1 + 2, 54);
			ssd1306_WriteString(options[i], Font_6x8, Black);
		}
		else {
			ssd1306_DrawRectangle(x1, 52, x2, 62, White);
			ssd1306_SetCursor(x1 + 2, 54);
			ssd1306_WriteString(options[i], Font_6x8, White);
		}

		nav_bar_option_index++;
		x1 = x2 + 2;
	}
	// ssd1306_UpdateScreen();
}


display_state display_in_charging_stats_two() {
	if (current_charger_state != CHARGER_STATE_CHARGING || 
		current_charger_state != CHARGER_STATE_BALANCING ||
		current_charger_state != CHARGER_STATE_BALANCING_ONLY) {
		return DISPLAY_STATE_NAVIGATION;
	}

	int number_of_options = 1;

	char sum_of_cells[50];
	char state_of_charge[50];
	char average_stats[50];
	char charging_info[50];

	sprintf(average_stats, "Avg V:%.3fV", currentBmsAndElconData.BMS_avgVolt);
	sprintf(charging_info, "%.2f V @ %.2f A", LIMIT_VOLTS, LIMIT_AMPS);
	sprintf(state_of_charge, "SOC:%.2f%%", currentBmsAndElconData.BMS_stateOfCharge);
	sprintf(sum_of_cells, "Pack Volt: %.2fV", currentBmsAndElconData.BMS_sumOfCells);

	display_clear();
	display_wrap_selected_option(number_of_options)

	bool is_balancing = (current_charging_mode == CHARGING_MODE_BALANCING);

	display_draw_title_bar(is_balancing ? "Balancing 2" : "Charging 2");

	ssd1306_SetCursor(1, 13);
	ssd1306_WriteString(sum_of_cells, Font_6x8, White);

	//Writes SOC Stats
	ssd1306_SetCursor(1, 23);
	ssd1306_WriteString(state_of_charge, Font_6x8, White);

	ssd1306_SetCursor(1, 33);
	ssd1306_WriteString(average_stats, Font_6x8, White);

	ssd1306_SetCursor(1, 43);
	ssd1306_WriteString(charging_info, Font_6x8, White);


	char *nav_bar_options[1] = { is_balancing ? "Balancing 1" : "Charging 1" };
	int first_nav_bar_option_index = 0;
	display_draw_nav_bar(nav_bar_options, number_of_options, first_nav_bar_option_index);

	if (select_pressed) {
		display_check_selected_option_bounds(number_of_options);
		switch(selected_option) {
			case 0: return DISPLAY_STATE_IN_CHARGING_STATS_TWO;
		}
	}
	return DISPLAY_STATE_IN_CHARGING_STATS_ONE;
}

display_state display_charging_profiles() {
	profile profiles[number_of_profiles];
	
	int profile_index = 0;
	for (int i = 0; i < number_of_profiles; i++) {
		if (all_profiles[i].voltage * all_profiles[i].current <= (MAX_ALLOWED_PWR * 97 / 100) && all_profiles[i].voltage > currentBmsAndElconData.BMS_sumOfCells) {
			profiles[profile_index] = all_profiles[i];
			profile_index++;
		}
	}

	int current_number_of_profiles = profile_index;

  	int navStartIndex = current_number_of_profiles;
  	int navLastIndex = current_number_of_profiles;

	

	//resets screen
	display_clear();

	display_draw_title_bar("Charging");

	int currentScreen = selected_option/3;


	//if the nav bar is selected, ensures that the currentScreen is the last screen of profiles
	if (selected_option > current_number_of_profiles-1) {
		currentScreen = (current_number_of_profiles-1)/3;
	}
	int startIndex = currentScreen*3;
	//ensures that the correct number of profiles are showed on the last screen
	if (selected_option > current_number_of_profiles-1) {
		startIndex = (current_number_of_profiles-1)/3*3;
	}
	//going up from first profile will go to Start button
	if (selected_option < 0) {
		startIndex = 0;
		selected_option = navLastIndex;
	}
	//going down from start button will reset back to first profile being selected
	if (selected_option > navLastIndex) {
		startIndex = 0;
		selected_option = 0;
	}

	//initial y-positions used for calculating profile display boxes
	int y1 = 15;
	int y2 = 13;
	int y3 = 24;

	//displays up to three profiles per screen
	for (int i = startIndex; i < startIndex + 3 && i < current_number_of_profiles; i++) {
		char profileString[50];
		sprintf(profileString, "%s: %dA %dV", profiles[i].name, profiles[i].current, profiles[i].voltage);
		ssd1306_SetCursor(3, y1);

		if (selected_option == i) {
			ssd1306_FillRectangle(1,y2, 122, y3, White);
			ssd1306_WriteString(profileString, Font_6x8, Black);
		}
		else {
			ssd1306_DrawRectangle(1,y2, 122, y3, White);
			ssd1306_WriteString(profileString, Font_6x8, White);
		}

		y1 = y1 + 13;
		y2 = y2 + 13;
		y3 = y3 + 13;
	}

	//numOfProiles + 2 ensures it will always round up
	int numOfScreens = (number_of_profiles+2)/3;

	display_draw_short_scroll_bar(currentScreen, numOfScreens);

	char *nav_bar_options[] = {"Nav"};
	int number_of_nav_bar_options = 1;
	display_draw_nav_bar(nav_bar_options, number_of_nav_bar_options, navStartIndex);

	ssd1306_UpdateScreen();

	if (select_pressed) {
	    // Make sure the selected option is within the valid range of profiles
	    if (selected_option >= 0 && selected_option < current_number_of_profiles) {
	        profile selected_profile = profiles[selected_option];
	        // Set charging limits based on the selected profile
			LIMIT_VOLTS = selectedProfile.voltage;
			LIMIT_AMPS = selectedProfile.current;
			return DISPLAY_STATE_CHARGING_INITIALIZATION;
	    }
	    else if (selected_option == current_number_of_profiles) {
			return DISPLAY_STATE_NAVIGATION;
	    }
	}
	return DISPLAY_STATE_CHARGING_PROFILES;
}

display_state display_charging_initialization() {
	display_clear();

	if (!charger_is_charger_safe()) {
    	ssd1306_SetCursor(5, 5);
		ssd1306_WriteString("HVIL ERROR", Font_6x8, White);
		ssd1306_UpdateScreen();
	}
	else if (!charger_is_hvil_switch_flipped()) {
		ssd1306_SetCursor(5, 5);
		ssd1306_WriteString("PLEASE FLIP HV", Font_6x8, White);
		ssd1306_UpdateScreen();
	}
	else if (!charger_is_ready_to_charge_switch_flipped()) {
		ssd1306_SetCursor(5, 5);
		ssd1306_WriteString("PLEASE FLIP RTC", Font_6x8, White);
		ssd1306_UpdateScreen();
	}
	else {
		current_charger_state = CHARGER_STATE_CHARGING;
		return DISPLAY_STATE_IN_CHARGING_STATS_ONE;
	}
	return DISPLAY_STATE_CHARGING_INITIALIZATION;
}

void display_draw_short_scroll_bar(int current_view, int number_of_views) {
	//current_view is zero-indexed
	if (number_of_views > 1) {
		int scroll_container_height = 35;
		int scroll_bar_length = scroll_container_height/number_of_views;
		int scroll_bar_start = 14 + (current_view * scroll_bar_length);

		ssd1306_DrawRectangle(124, 13, 126, 47, White);
		ssd1306_Line(125, scroll_bar_start, 125, scroll_bar_start + scroll_bar_length, White);
	}

}

void display_draw_long_scroll_bar(int current_view, int number_of_views) {
	//currentScreen is zero-indexed
	if (number_of_views > 1) {
		int scroll_container_height = 49;
		int scroll_bar_length = scroll_container_height/number_of_views;
		int scroll_bar_start = 14+ (current_view * number_of_views);

		ssd1306_DrawRectangle(124, 13, 126, 62, White);
		ssd1306_Line(125, scroll_bar_start, 125, scroll_bar_start + scroll_bar_length, White);
	}
}

display_state display_charger_stats() {
	char inletTempString[50];
	char outletTempString[50];
	int number_of_options = 2;

	display_clear();
	display_wrap_selected_option(number_of_options);

	sprintf(inletTempString, "Inlet Tmp:%.2f", READ_THERM(therm_inlet, THERM_RESIST));
	sprintf(outletTempString, "Outlet Tmp:%.2f", READ_THERM(therm_outlet, THERM_RESIST));

	display_draw_title_bar("Charger Stats");

	ssd1306_SetCursor(1, 13);
	ssd1306_WriteString(inletTempString, Font_6x8, White);

	ssd1306_SetCursor(1, 23);
	ssd1306_WriteString(outletTempString, Font_6x8, White);

	ssd1306_SetCursor(1, 33);
	if (isError) {
		ssd1306_WriteString("Errors detected", Font_6x8, White);
	}
	else {
		ssd1306_WriteString("No errors detected", Font_6x8, White);
	}


	char *nav_bar_options[] = {"Nav", "Batt"};
	int number_of_nav_bar_options = 2;
	int first_nav_bar_option_index = 0;
	display_draw_nav_bar(nav_bar_options, number_of_nav_bar_options, first_nav_bar_option_index);

	ssd1306_UpdateScreen();

	if (select_pressed) {
		display_check_selected_option_bounds(number_of_options);
		switch (selected_option) {
			case 0: return DISPLAY_STATE_NAVIGATION;
			case 1: return DISPLAY_STATE_BATTERY_STATS_ONE;
		}
	}
	return DISPLAY_STATE_CHARGER_STATS;
}

void display_battery_stats_one(){
	char temperature_stats[50];
	char voltage_stats[50];
	char average_stats[50];
	int number_of_options = 2;

	sprintf(temperature_stats, "Tmp H/L:%.2f/%.2fC",
		currentBmsAndElconData.BMS_maxTemp,
		currentBmsAndElconData.BMS_minTemp);
	sprintf(voltage_stats, "Vlt H/L:%.3f/%.3fV",
		currentBmsAndElconData.BMS_maxVolt,
		currentBmsAndElconData.BMS_minVolt);
	sprintf(average_stats, "Avg V:%.3fV",
		currentBmsAndElconData.BMS_avgVolt);

	display_clear();
	display_wrap_selected_option(number_of_options);

	display_draw_title_bar("Battery 1");

	ssd1306_SetCursor(1, 13);
	ssd1306_WriteString(temperature_stats, Font_6x8, White);

	ssd1306_SetCursor(1, 23);
	ssd1306_WriteString(voltage_stats, Font_6x8, White);

	ssd1306_SetCursor(1, 33);
	ssd1306_WriteString(average_stats, Font_6x8, White);

	char *nav_bar_options[] = {"Nav", "Battery 2"};
	int first_nav_bar_option_index = 0;

	display_draw_nav_bar(nav_bar_options, number_of_options, first_nav_bar_option_index);

	ssd1306_UpdateScreen();
	

	if (select_pressed) {
		display_check_selected_option_bounds(number_of_options);
		switch(selected_option) {
			case 0: return DISPLAY_STATE_NAVIGATION;
			case 1: return DISPLAY_STATE_BATTERY_STATS_TWO;
		}
	}
	return DISPLAY_STATE_BATTERY_STATS_ONE;
}


display_state display_battery_stats_two(){

	char state_of_charge[50];
	char pack_volt[50];
	char pack_imbalance[50];
	int number_of_options = 2;

	sprintf(state_of_charge, "SOC:%.2f%%",
			currentBmsAndElconData.BMS_stateOfCharge);
			
	sprintf(pack_imbalance, "Imbalance:%.2fV",
			currentBmsAndElconData.BMS_packImbalance);
	
	sprintf(pack_volt, "Pack Volt: %.2fV",
			currentBmsAndElconData.BMS_sumOfCells);


	display_clear();
	display_wrap_selected_option(number_of_options);

	display_draw_title_bar("Battery 2");

	ssd1306_SetCursor(1, 13);
	ssd1306_WriteString(state_of_charge, Font_6x8, White);

	ssd1306_SetCursor(1, 23);
	ssd1306_WriteString(pack_imbalance, Font_6x8, White);

	ssd1306_SetCursor(1, 33);
	ssd1306_WriteString(pack_volt, Font_6x8, White);

	
	char *nav_bar_options[] = {"Nav", "Battery 1"};
	int first_nav_bar_option_index = 0;
	display_draw_nav_bar(nav_bar_options, number_of_options, first_nav_bar_option_index);

	ssd1306_UpdateScreen();
	

	if (select_pressed) {
		display_wrap_selected_option(number_of_options);
		switch(selected_option) {
			case 0: return DISPLAY_STATE_NAVIGATION;
			case 1: return DISPLAY_STATE_BATTERY_STATS_ONE;
		}
	}
	return DISPLAY_STATE_BATTERY_STATS_TWO;
}

//Display Start Balancing
display_state display_start_balancing() {
	int number_of_options = 2;

	display_clear();
	display_wrap_selected_option(number_of_options)

	display_draw_title_bar("Start Balancing");

	ssd1306_SetCursor(1, 13);
	ssd1306_WriteString("Balancing is off", Font_6x8, White);

	char *nav_bar_options[] = {"Nav", "Start Bal"};
	int nav_bar_start_index = 0;
	display_draw_nav_bar(nav_bar_options, number_of_options, nav_bar_start_index);


	ssd1306_UpdateScreen();
	
	if (select_pressed) {
		display_check_selected_option_bounds(number_of_options);
		switch (selected_option) {
			case 0: return DISPLAY_STATE_NAVIGATION;
			case 1: {
				return DISPLAY_STATE_BALANCING_INITIALIZATION;
			}
		}
	}
	return DISPLAY_STATE_START_BALANCING;
}

display_state display_balancing_initialization() {
	display_clear();
	if (!charger_is_charger_safe()) {
    	ssd1306_SetCursor(5, 5);
		ssd1306_WriteString("HVIL ERROR", Font_6x8, White);
		ssd1306_UpdateScreen();
	}
	else if (!charger_is_hvil_switch_flipped()) {
		ssd1306_SetCursor(5, 5);
		ssd1306_WriteString("PLEASE FLIP HV", Font_6x8, White);
		ssd1306_UpdateScreen();
	}
	else {
		current_charger_state = CHARGER_STATE_BALANCING;
		return DISPLAY_STATE_IN_CHARGING_STATS_ONE;
	}
	return DISPLAY_STATE_BALANCING_INITIALIZATION;
}

void display_draw_title_bar(char title[]) {
	static uint32_t previous_time = 0;

	uint32_t current_time = HAL_GetTick();
	if (current_time - previous_time >= 1000) {
		ssd1306_DrawPixel(85, 5, White);
		previous_time = current_time;
	}
	else {
		ssd1306_DrawPixel(85, 5, Black);
	}

	isError = false;
	for (int i = 0; i < 5; i++) {
		if (currentBmsAndElconData.ELCON_fault[i] == 1) {
			isError = true;
			break;
		}
	}

	ssd1306_SetCursor(1, 1);
	ssd1306_WriteString(title, Font_6x8, White);
	ssd1306_Line(0, 10, 127, 10, White);

	//Flashing status symbols 
	// ssd1306_FillRectangle(70, 0, 127, 9, Black);
	// ssd1306_UpdateScreen();

	if (isError) {
		display_draw_error_symbol(119,1);
		if (isCharging) {
			display_draw_in_charging_symbol(92, 3);
			if (isBalancing) {
				ssd1306_FillRectangle(71, 0, 89, 8, White);
				ssd1306_SetCursor(72, 1);
				ssd1306_WriteString("BAL", Font_6x8, Black);
			}
		}
		else if (isBalancing) {
			ssd1306_FillRectangle(91, 0, 109, 8, White);
			ssd1306_SetCursor(92, 1);
			ssd1306_WriteString("BAL", Font_6x8, Black);
		}
	}
	else if (isCharging) {
		display_draw_in_charging_symbol(109, 3);
		if (isBalancing) {
			ssd1306_FillRectangle(88, 0, 106, 8, White);
			ssd1306_SetCursor(89, 1);
			ssd1306_WriteString("BAL", Font_6x8, Black);
		}
	}
	else if (isBalancing) {
		ssd1306_FillRectangle(108, 0, 126, 8, White);
		ssd1306_SetCursor(109, 1);
		ssd1306_WriteString("BAL", Font_6x8, Black);
	}
}

void display_draw_in_charging_symbol(int x, int y) {
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

void display_draw_error_symbol(int x, int y) {
	//point of origin (x,y) is the top of the triangle
	ssd1306_Line(x, y, x+7, y+7, White);
	ssd1306_Line(x, y, x-7, y+7, White);
	ssd1306_Line(x-7, y+7, x+7, y+7, White);

	ssd1306_Line(x, y+2, x, y+4, White);
	ssd1306_Line(x, y+6, x, y+6, White);
}

display_state display_in_charging_stats_one() {
	if (current_charger_state != CHARGER_STATE_CHARGING || 
		current_charger_state != CHARGER_STATE_BALANCING ||
		current_charger_state != CHARGER_STATE_BALANCING_ONLY) {
		return DISPLAY_STATE_NAVIGATION;
	}
	int number_of_options = 1;
	char temperature_stats[50];
	char voltage_stats[50];
	char imbalance[30];
	char output_stats[50];

	sprintf(temperature_stats, "Tmp H/L:%.2f/%.2fC",
			currentBmsAndElconData.BMS_maxTemp,
			currentBmsAndElconData.BMS_minTemp);
	sprintf(voltage_stats, "Vlt H/L:%.3f/%.3fV",
			currentBmsAndElconData.BMS_maxVolt,
			currentBmsAndElconData.BMS_minVolt);
	sprintf(imbalance, "Imbal:%.3fV", currentBmsAndElconData.BMS_packImbalance);
	sprintf(output_stats, "Out V/C:%.2fV/%.2fA", currentBmsAndElconData.ELCON_outVolt, currentBmsAndElconData.ELCON_outCurrent);

	display_clear();
	display_wrap_selected_option(number_of_options);
	
	bool is_balancing = (current_charging_mode == CHARGING_MODE_BALANCING);
	//Writes title
	display_draw_title_bar(is_balancing ? "Balancing 1" : "Charging 1");

	//Writes temp
	ssd1306_SetCursor(1, 13);
	ssd1306_WriteString(temperature_stats, Font_6x8, White);

	//Writes voltage
	ssd1306_SetCursor(1, 23);
	ssd1306_WriteString(voltage_stats, Font_6x8, White);

	//Writes imbalance
	ssd1306_SetCursor(1, 33);
	ssd1306_WriteString(imbalance, Font_6x8, White);

	//Writes output info
	if (!is_balancing) {
		ssd1306_SetCursor(1, 43);
		ssd1306_WriteString(output_stats, Font_6x8, White);
	}
	
	char *nav_bar_options[1] = { is_balancing ? "Balancing 2" : "Charging 2" };

	int first_nav_bar_option_index = 0;
	display_draw_nav_bar(nav_bar_options, number_of_options, first_nav_bar_option_index);

	if (select_pressed) {
		display_check_selected_option_bounds(number_of_options);
		switch (selected_option) {
			case 0: return DISPLAY_STATE_IN_CHARGING_STATS_TWO;
		}
	}
	return DISPLAY_STATE_IN_CHARGING_STATS_ONE;
}

display_state display_errors() {
	
	const char *error_messages[5] = {
		"HW Fail",
		"Charger Overtemp",
		"Wrong Input Volt",
		"No Batt Volt",
		"Comms Timeout"
	};

	char current_errors[5][100];
	int current_error_index = 0;

	for (int i = 0; i < 5; i++) {
		if (currentBmsAndElconData.ELCON_fault[i] == 1) {
			sprintf(current_errors[current_error_index], "%s", error_messages[i]);
			current_error_index++;
		}
	}

	int number_of_errors = current_error_index;

	int nav_bar_start_index = number_of_errors;
	int nav_bar_last_index = number_of_errors + 1;
	int number_of_options = nav_bar_last_index;

	display_clear();

	display_draw_title_bar("Errors");

	
	int current_view = selected_option/3;


	//if the nav bar is selected, ensures that the currentScreen is the last screen of profiles
	if (selected_option > number_of_errors - 1) {
		current_view = (number_of_errors -1 ) / 3;
	}
	int start_index = current_view * 3;
	//ensures that the correct number of profiles are showed on the last screen
	if (selected_option > number_of_errors - 1) {
		start_index = (number_of_errors-1) / 3 * 3;
	}
	//going up from first profile will go to Start button
	if (selected_option < 0) {
		start_index = 0;
		selected_option = nav_bar_last_index;
	}
	//going down from start button will reset back to first profile being selected
	if (selected_option > nav_bar_last_index) {
		start_index = 0;
		selected_option = 0;
	}

	//initial y-positions used for calculating profile display boxes
	int y1 = 15;
	int y2 = 13;
	int y3 = 24;

	//displays up to three errors per screen
	for (int i = start_index; i < start_index + 3 && i < number_of_errors; i++) {
		ssd1306_SetCursor(3, y1);
		if (selected_option == i) {
			ssd1306_FillRectangle(1,y2, 122, y3, White);
			ssd1306_WriteString(current_errors[i], Font_6x8, Black);
		}
		else {
			ssd1306_DrawRectangle(1,y2, 122, y3, White);
			ssd1306_WriteString(current_errors[i], Font_6x8, White);
		}

		y1 = y1 + 13;
		y2 = y2 + 13;
		y3 = y3 + 13;
	}

	int number_of_views= (number_of_errors + 2) / 3; //+ 2 ensures it will always round up

	display_draw_short_scroll_bar(current_view, number_of_views);

	char *nav_bar_options[] = {"Nav"};
	int number_of_nav_bar_options = 1;
	display_draw_nav_bar(nav_bar_options, number_of_nav_bar_options, nav_bar_start_index);

	ssd1306_UpdateScreen();

	if (select_pressed) {
		display_check_selected_option_bounds(number_of_options);
		if (selected_option < nav_bar_start_index && selected_option >= 0 ) {
			return DISPLAY_STATE_ERRORS;
		}
		if (selected_option == nav_bar_start_index) {
			return DISPLAY_STATE_NAVIGATION;
		}
	}
	return DISPLAY_STATE_ERRORS;
}
