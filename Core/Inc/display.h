// Written by Ayman Alamayri in Dec 2024
#ifndef DISPLAY_H
#define DISPLAY_H

#include <string.h>
#include <stdbool.h>

#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "graphics.h"




typedef enum {
	DISPLAY_STATE_NAVIGATION,
	DISPLAY_STATE_HOME,
	DISPLAY_STATE_CHARGING_PROFILES,
	DISPLAY_STATE_START_BALANCING,
	DISPLAY_STATE_BATTERY_STATS_ONE,
	DISPLAY_STATE_BATTERY_STATS_TWO,
	DISPLAY_STATE_CHARGER_STATS,
	DISPLAY_STATE_IN_CHARGING_STATS_ONE,
	DISPLAY_STATE_IN_CHARGING_STATS_TWO,
	DISPLAY_STATE_ERRORS,
} display_state;

extern display_state next_display_state;
extern display_state current_display_state;
extern bool select_pressed;
extern int selected_option;
extern bool back_pressed;

void DISP_KanoaSplash();

void display_init();
display_state display_update_state()
display_state display_navigation();
display_state display_home();
void display_draw_nav_bar(char *options[], int number_of_nav_bar_options, int first_nav_bar_option_index);
display_state display_errors();
void SRE_Display_Start_Balancing();
display_state display_in_charging_stats_one();
display_state display_battery_stats_two();
display_state display_in_charging_stats_two();
void SRE_Display_Start_Charging();
void display_draw_short_scroll_bar(int current_view, int number_of_views);
void display_draw_long_scroll_bar(int current_view, int number_of_views);
display_state display_charger_stats();
display_state display_battery_stats_one();
void display_draw_title_bar(char title[]);
void display_draw_in_charging_symbol(int x, int y);
void display_draw_error_symbol(int x, int y);
void display_check_selected_option_bounds(int number_of_options);
void display_wrap_selected_option(int number_of_options);

#endif
