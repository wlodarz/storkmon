
#include <PalmOS.h>
#include <stdio.h>
#include <string.h>
#include "draw.h"

#define DISPLAY_WINDOW_WIDTH 120
#define NUM_VAL_LEN 8

struct draw_state {
	UInt32 new_temp; // recently added temperature value
	UInt32 new_windspeed; // recently added windspeed value

	int start_index; // what is starting point

	signed char temperature_string[NUM_VAL_LEN];
	signed char windspeed_string[NUM_VAL_LEN];

	signed char temp_limit_min_string[NUM_VAL_LEN];
	signed char temp_limit_max_string[NUM_VAL_LEN];
	signed char wind_limit_min_string[NUM_VAL_LEN];
	signed char wind_limit_max_string[NUM_VAL_LEN];

	int first_temp_set;
	int first_wind_set;

	UInt32 limit_temp_min, limit_temp_max;
	UInt32 limit_wind_min, limit_wind_max;
	UInt32 old_limit_temp_min, old_limit_temp_max;
	UInt32 old_limit_wind_min, old_limit_wind_max;

	UInt32 temperature_table[DISPLAY_WINDOW_WIDTH];
	UInt32 windspeed_table[DISPLAY_WINDOW_WIDTH];

	int constant_drawed;

	int limit_selected;
	int limit_selected_timeout;
	RectangleType selection_rect;
	int limit;
};

static struct draw_state draw_st;

/* init draw module
 */
int draw_init(void)
{

	draw_st.start_index = 0;
	draw_st.constant_drawed = 0;
	draw_st.first_wind_set = 0;
	draw_st.first_temp_set = 0;
	memset((void *)&draw_st.temperature_table[0], 0, sizeof(draw_st.temperature_table));
	memset((void *)&draw_st.windspeed_table[0], 0, sizeof(draw_st.windspeed_table));
	*(char *)draw_st.temperature_string = '\0';
	*(char *)draw_st.windspeed_string = '\0';
	draw_st.limit_selected = 0;
	draw_st.limit_selected_timeout = 0;

	return 0;
}

int draw_constant(void)
{
        char *wind_str = "WIND";
        char *temp_str = "TEMP";

	WinDrawChars(wind_str, strlen(wind_str), ELEM_POS_TEMP_NUM_X, (DISPLAY_WINDOW_HEIGHT / 2) + 10);
	WinDrawChars(temp_str, strlen(temp_str), ELEM_POS_WIND_NUM_X, 15);

	draw_st.constant_drawed = 1;
}

/* destroy draw module
 */
int draw_destroy(void)
{
	return 0;
}

void draw_print_wind_limits(void)
{
	int val;
	WinEraseChars(draw_st.wind_limit_min_string, strlen(draw_st.wind_limit_min_string), ELEM_POS_WIND_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_WIND_MIN_NUM_Y);
	WinEraseChars(draw_st.wind_limit_max_string, strlen(draw_st.wind_limit_max_string), ELEM_POS_WIND_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_WIND_MAX_NUM_Y);

	val = draw_st.limit_wind_min;
	sprintf(draw_st.wind_limit_min_string, "%02d.%02d", val / 100, val % 100);
	val = draw_st.limit_wind_max;
	sprintf(draw_st.wind_limit_max_string, "%02d.%02d", val / 100, val % 100);
	WinDrawChars(draw_st.wind_limit_min_string, strlen(draw_st.wind_limit_min_string), ELEM_POS_WIND_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_WIND_MIN_NUM_Y);
	WinDrawChars(draw_st.wind_limit_max_string, strlen(draw_st.wind_limit_max_string), ELEM_POS_WIND_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_WIND_MAX_NUM_Y);
}

void draw_print_temp_limits(void)
{
	int val;

	WinEraseChars(draw_st.temperature_string, strlen(draw_st.temperature_string), ELEM_POS_TEMP_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_TEMP_MIN_NUM_Y);
	WinEraseChars(draw_st.temperature_string, strlen(draw_st.temperature_string), ELEM_POS_TEMP_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_TEMP_MAX_NUM_Y);

	val = draw_st.limit_temp_min;
	sprintf(draw_st.temp_limit_min_string, "%02d.%02d", val / 100, val % 100);
	val = draw_st.limit_temp_max;
	sprintf(draw_st.temp_limit_max_string, "%02d.%02d", val / 100, val % 100);
	WinDrawChars(draw_st.temp_limit_min_string, strlen(draw_st.temp_limit_min_string), ELEM_POS_TEMP_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_TEMP_MIN_NUM_Y);
	WinDrawChars(draw_st.temp_limit_max_string, strlen(draw_st.temp_limit_max_string), ELEM_POS_TEMP_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_TEMP_MAX_NUM_Y);
}

/* add measured temperature value to the screen
 */
void draw_addval_temp(int y)
{
	draw_st.new_temp = y;
}

/* add measured windspeed value to the screen
 */
void draw_addval_wind(int y)
{
	draw_st.new_windspeed = y;
}

/* update screen
 */
void draw_update(void)
{
	int x;
	int tval, wval;

	draw_constant();

	if (!draw_st.first_temp_set) {
		UInt32 new_temp = (draw_st.new_temp / 100) * 100;
		draw_st.limit_temp_min = (new_temp >= 100) ? (new_temp - 100) : 0;
		draw_st.old_limit_temp_min = draw_st.limit_temp_min;
		draw_st.limit_temp_max = new_temp + 200;
		draw_st.old_limit_temp_max = draw_st.limit_temp_max;
		memset((void *)&draw_st.temperature_table[0], draw_st.new_temp, sizeof(draw_st.temperature_table));
		draw_print_temp_limits();
		draw_st.first_temp_set = 1;
	}

	if (!draw_st.first_wind_set) {
		UInt32 new_windspeed = (draw_st.new_windspeed / 100) * 100;
		draw_st.limit_wind_min = (new_windspeed >= 100) ? (new_windspeed - 100) : 0;
		draw_st.old_limit_wind_min = draw_st.limit_wind_min;
		draw_st.limit_wind_max = new_windspeed + 200;
		draw_st.old_limit_wind_max = draw_st.limit_wind_max;
		memset((void *)&draw_st.windspeed_table[0], draw_st.new_windspeed, sizeof(draw_st.windspeed_table));
		draw_print_wind_limits();
		draw_st.first_wind_set = 1;
	}

	// first clean last drawing, both: temperature and windspeed
	for(x=0; x<DISPLAY_WINDOW_WIDTH; x++) {
		UInt32 xp, yp;
		UInt32 temp_y, wind_y;
		UInt32 temp_delta = draw_st.old_limit_temp_max - draw_st.old_limit_temp_min;
		UInt32 wind_delta = draw_st.old_limit_wind_max - draw_st.old_limit_wind_min;
		UInt32 display_height;

		if (draw_st.first_temp_set) {
		display_height = ELEM_TEMP_HEIGHT;
		xp = x + ELEM_POS_TEMP_GRAPH_X;
		temp_y = draw_st.temperature_table[(x+draw_st.start_index) % DISPLAY_WINDOW_WIDTH];
		temp_y = (temp_y < draw_st.old_limit_temp_min) ? draw_st.old_limit_temp_min : temp_y;
		temp_y = (temp_y > draw_st.old_limit_temp_max) ? draw_st.old_limit_temp_max : temp_y;
		temp_y = display_height * (temp_y - draw_st.old_limit_temp_min) / temp_delta;
		yp = DISPLAY_WINDOW_HEIGHT - ELEM_POS_TEMP_GRAPH_Y - temp_y;
		WinErasePixel(xp, yp);
		}

		if (draw_st.first_wind_set) {
		display_height = ELEM_WIND_HEIGHT;
		xp = x + ELEM_POS_WIND_GRAPH_X;
		wind_y = draw_st.windspeed_table[(x+draw_st.start_index) % DISPLAY_WINDOW_WIDTH];
		wind_y = (wind_y < draw_st.old_limit_wind_min) ? draw_st.old_limit_wind_min : wind_y;
		wind_y = (wind_y > draw_st.old_limit_wind_max) ? draw_st.old_limit_wind_max : wind_y;
		wind_y = display_height * (wind_y - draw_st.old_limit_wind_min) / wind_delta;
		yp = DISPLAY_WINDOW_HEIGHT - ELEM_POS_WIND_GRAPH_Y - wind_y;
		WinErasePixel(xp, yp);
		}
	}
	// clean also last numeric values
	if (draw_st.first_temp_set)
		WinEraseChars(draw_st.temperature_string, strlen(draw_st.temperature_string), ELEM_POS_TEMP_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_TEMP_NUM_Y);
	if (draw_st.first_wind_set)
		WinEraseChars(draw_st.windspeed_string, strlen(draw_st.temperature_string), ELEM_POS_WIND_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_WIND_NUM_Y);

	// add new values
	draw_st.temperature_table[(x+draw_st.start_index)%DISPLAY_WINDOW_WIDTH] = draw_st.new_temp;
	draw_st.windspeed_table[(x+draw_st.start_index)%DISPLAY_WINDOW_WIDTH] = draw_st.new_windspeed;

	// move start index
	draw_st.start_index = (draw_st.start_index + 1) % DISPLAY_WINDOW_WIDTH;

	// now draw new drawing, both: temperature and windspeed
	for(x=0; x<DISPLAY_WINDOW_WIDTH; x++) {
		UInt32 xp, yp;
		UInt32 temp_y, wind_y;
		UInt32 temp_delta = draw_st.old_limit_temp_max - draw_st.old_limit_temp_min;
		UInt32 wind_delta = draw_st.old_limit_wind_max - draw_st.old_limit_wind_min;
		UInt32 display_height;

		if (draw_st.first_temp_set) {
		display_height = ELEM_TEMP_HEIGHT;
		xp = x + ELEM_POS_TEMP_GRAPH_X;
		temp_y = draw_st.temperature_table[(x+draw_st.start_index) % DISPLAY_WINDOW_WIDTH];
		temp_y = (temp_y < draw_st.limit_temp_min) ? draw_st.limit_temp_min : temp_y;
		temp_y = (temp_y > draw_st.limit_temp_max) ? draw_st.limit_temp_max : temp_y;
		temp_y = display_height * (temp_y - draw_st.limit_temp_min) / temp_delta;
		yp = DISPLAY_WINDOW_HEIGHT - ELEM_POS_TEMP_GRAPH_Y - temp_y;
		WinDrawPixel(xp, yp);
		draw_st.old_limit_temp_min = draw_st.limit_temp_min;
		draw_st.old_limit_temp_max = draw_st.limit_temp_max;
		}

		if (draw_st.first_wind_set) {
		display_height = ELEM_WIND_HEIGHT;
		xp = x + ELEM_POS_WIND_GRAPH_X;
		wind_y = draw_st.windspeed_table[(x+draw_st.start_index) % DISPLAY_WINDOW_WIDTH];
		wind_y = (wind_y < draw_st.limit_wind_min) ? draw_st.limit_wind_min : wind_y;
		wind_y = (wind_y > draw_st.limit_wind_max) ? draw_st.limit_wind_max : wind_y;
		wind_y = display_height * (wind_y - draw_st.limit_wind_min) / wind_delta;
		yp = DISPLAY_WINDOW_HEIGHT - ELEM_POS_WIND_GRAPH_Y - wind_y;
		WinDrawPixel(xp, yp);

		draw_st.old_limit_wind_min = draw_st.limit_wind_min;
		draw_st.old_limit_wind_max = draw_st.limit_wind_max;
		}
	}
	// draw also new numeric values
	tval = draw_st.new_temp;
	wval = draw_st.new_windspeed;
	sprintf(draw_st.temperature_string, "%02d.%02d", tval / 100, tval % 100);
	sprintf(draw_st.windspeed_string,   "%02d.%02d", wval / 100, wval % 100);
	if (draw_st.first_temp_set)
		WinDrawChars(draw_st.temperature_string, strlen(draw_st.temperature_string), ELEM_POS_TEMP_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_TEMP_NUM_Y);
	if (draw_st.first_wind_set)
		WinDrawChars(draw_st.windspeed_string, strlen(draw_st.temperature_string), ELEM_POS_WIND_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_WIND_NUM_Y);
}

void draw_paint_selection(int x, int y, int dx, int dy)
{
	WinDrawLine(x,    y, x+dx, y);
	WinDrawLine(x+dx, y, x+dx, y+dy);
	WinDrawLine(x+dx, y+dy, x, y+dy);
	WinDrawLine(x, y+dy, x, y);
}
void draw_erase_selection(int x, int y, int dx, int dy)
{
	WinEraseLine(x,    y, x+dx, y);
	WinEraseLine(x+dx, y, x+dx, y+dy);
	WinEraseLine(x+dx, y+dy, x, y+dy);
	WinEraseLine(x, y+dy, x, y);
}

void draw_select_limit(int limit)
{
	int x, y, dx, dy;

	draw_st.limit = limit;
	/* clear previous selection */
	if (draw_st.limit_selected) {
		x = draw_st.selection_rect.topLeft.x;
		y = draw_st.selection_rect.topLeft.y;
		dx = draw_st.selection_rect.extent.x;
		dy = draw_st.selection_rect.extent.y;
		draw_erase_selection(x, y, dx, dy);
	}

	switch (limit) {
		case LIMIT_WIND_MAX:
			draw_st.selection_rect.topLeft.x = LIMIT_WIND_MAX_X;
			draw_st.selection_rect.topLeft.y = LIMIT_WIND_MAX_Y;
			draw_st.selection_rect.extent.x = LIMIT_WIDTH;
			draw_st.selection_rect.extent.y = LIMIT_HEIGHT;
			break;
		case LIMIT_WIND_MIN:
			draw_st.selection_rect.topLeft.x = LIMIT_WIND_MIN_X;
			draw_st.selection_rect.topLeft.y = LIMIT_WIND_MIN_Y;
			draw_st.selection_rect.extent.x = LIMIT_WIDTH;
			draw_st.selection_rect.extent.y = LIMIT_HEIGHT;
			break;
		case LIMIT_TEMP_MAX:
			draw_st.selection_rect.topLeft.x = LIMIT_TEMP_MAX_X;
			draw_st.selection_rect.topLeft.y = LIMIT_TEMP_MAX_Y;
			draw_st.selection_rect.extent.x = LIMIT_WIDTH;
			draw_st.selection_rect.extent.y = LIMIT_HEIGHT;
			break;
		case LIMIT_TEMP_MIN:
			draw_st.selection_rect.topLeft.x = LIMIT_TEMP_MIN_X;
			draw_st.selection_rect.topLeft.y = LIMIT_TEMP_MIN_Y;
			draw_st.selection_rect.extent.x = LIMIT_WIDTH;
			draw_st.selection_rect.extent.y = LIMIT_HEIGHT;
			break;
		default:
			return;
	}
	/* select limit by drawing frame */
	draw_st.limit_selected = 1;
	draw_st.limit_selected_timeout = 6;

	x = draw_st.selection_rect.topLeft.x;
	y = draw_st.selection_rect.topLeft.y;
	dx = draw_st.selection_rect.extent.x;
	dy = draw_st.selection_rect.extent.y;
	draw_paint_selection(x, y, dx, dy);
}

void draw_inc_limit(void)
{
	if (!draw_st.limit_selected) return;
	switch(draw_st.limit) {
		case LIMIT_WIND_MAX:
			if (draw_st.limit_wind_max < 9900) draw_st.limit_wind_max += 100;
			break;
		case LIMIT_WIND_MIN:
			if ((draw_st.limit_wind_min + 100) < draw_st.limit_wind_max) draw_st.limit_wind_min += 100;
			break;
		case LIMIT_TEMP_MAX:
			if (draw_st.limit_temp_max < 9900) draw_st.limit_temp_max += 100;
			break;
		case LIMIT_TEMP_MIN:
			if ((draw_st.limit_temp_min + 100) < draw_st.limit_temp_max) draw_st.limit_temp_min += 100;
			break;
	}
	draw_st.limit_selected_timeout = 6;
	draw_print_temp_limits();
	draw_print_wind_limits();
}

void draw_dec_limit(void)
{
	if (!draw_st.limit_selected) return;
	switch(draw_st.limit) {
		case LIMIT_WIND_MAX:
			if ((draw_st.limit_wind_max-100) > draw_st.limit_wind_min) draw_st.limit_wind_max -= 100;
			break;
		case LIMIT_WIND_MIN:
			if (draw_st.limit_wind_min > 100) draw_st.limit_wind_min -= 100;
			break;
		case LIMIT_TEMP_MAX:
			if ((draw_st.limit_temp_max-100) > draw_st.limit_temp_min) draw_st.limit_temp_max -= 100;
			break;
		case LIMIT_TEMP_MIN:
			if (draw_st.limit_temp_min > 100) draw_st.limit_temp_min -= 100;
			break;
	}
	draw_st.limit_selected_timeout = 6;
	draw_print_temp_limits();
	draw_print_wind_limits();
}

void draw_deselect_limit(void)
{
	UInt16 cornerDiam = 1;
	int x, y, dx, dy;

	/* clear previous selection */
	if (!draw_st.limit_selected) return;

	if (draw_st.limit_selected_timeout > 0)	draw_st.limit_selected_timeout--;
	if (draw_st.limit_selected_timeout > 0) return;

	draw_st.limit_selected = 0;
	/* clear previous selection */
	x = draw_st.selection_rect.topLeft.x;
	y = draw_st.selection_rect.topLeft.y;
	dx = draw_st.selection_rect.extent.x;
	dy = draw_st.selection_rect.extent.y;
	draw_erase_selection(x, y, dx, dy);
}
