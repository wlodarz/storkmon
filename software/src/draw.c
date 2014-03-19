
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

	signed char temperature_min_string[NUM_VAL_LEN];
	signed char temperature_max_string[NUM_VAL_LEN];
	signed char windspeed_min_string[NUM_VAL_LEN];
	signed char windspeed_max_string[NUM_VAL_LEN];

	UInt32 temp_min, temp_max;
	UInt32 wind_min, wind_max;
	int temp_min_set, temp_max_set;
	int wind_min_set, wind_max_set;
	int minmax_set;

	UInt32 limit_temp_min, limit_temp_max;
	UInt32 limit_wind_min, limit_wind_max;
	UInt32 old_limit_temp_min, old_limit_temp_max;
	UInt32 old_limit_wind_min, old_limit_wind_max;

	UInt32 temperature_table[DISPLAY_WINDOW_WIDTH];
	UInt32 windspeed_table[DISPLAY_WINDOW_WIDTH];

	int constant_drawed;
};

static struct draw_state draw_st;

/* init draw module
 */
int draw_init(void)
{

	draw_st.start_index = 0;
	draw_st.constant_drawed = 0;
	draw_st.temp_min = 9900;
	draw_st.temp_max = 0;
	draw_st.wind_min = 9900;
	draw_st.wind_max = 0;
	draw_st.temp_min_set = 0;
	draw_st.temp_max_set = 0;
	draw_st.wind_min_set = 0;
	draw_st.wind_max_set = 0;
	draw_st.minmax_set = 0;
	memset((void *)&draw_st.temperature_table[0], 0, sizeof(draw_st.temperature_table));
	memset((void *)&draw_st.windspeed_table[0], 0, sizeof(draw_st.windspeed_table));
	*(char *)draw_st.temperature_string = '\0';
	*(char *)draw_st.windspeed_string = '\0';


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

void draw_print_wind_minmax(void)
{
	int val;
	WinEraseChars(draw_st.windspeed_min_string, strlen(draw_st.windspeed_min_string), ELEM_POS_WIND_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_WIND_MIN_NUM_Y);
	WinEraseChars(draw_st.windspeed_max_string, strlen(draw_st.windspeed_max_string), ELEM_POS_WIND_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_WIND_MAX_NUM_Y);

	val = draw_st.wind_min;
	sprintf(draw_st.windspeed_min_string, "%02d.%02d", val / 100, val % 100);
	val = draw_st.wind_max;
	sprintf(draw_st.windspeed_max_string, "%02d.%02d", val / 100, val % 100);
	WinDrawChars(draw_st.windspeed_min_string, strlen(draw_st.windspeed_min_string), ELEM_POS_WIND_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_WIND_MIN_NUM_Y);
	WinDrawChars(draw_st.windspeed_max_string, strlen(draw_st.windspeed_max_string), ELEM_POS_WIND_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_WIND_MAX_NUM_Y);
}

void draw_print_temp_minmax(void)
{
	int val;

	WinEraseChars(draw_st.temperature_string, strlen(draw_st.temperature_string), ELEM_POS_TEMP_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_TEMP_MIN_NUM_Y);
	WinEraseChars(draw_st.temperature_string, strlen(draw_st.temperature_string), ELEM_POS_TEMP_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_TEMP_MAX_NUM_Y);

	val = draw_st.temp_min;
	sprintf(draw_st.temperature_min_string, "%02d.%02d", val / 100, val % 100);
	val = draw_st.temp_max;
	sprintf(draw_st.temperature_max_string, "%02d.%02d", val / 100, val % 100);
	WinDrawChars(draw_st.temperature_min_string, strlen(draw_st.temperature_min_string), ELEM_POS_TEMP_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_TEMP_MIN_NUM_Y);
	WinDrawChars(draw_st.temperature_max_string, strlen(draw_st.temperature_max_string), ELEM_POS_TEMP_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_TEMP_MAX_NUM_Y);
}

/* add measured temperature value to the screen
 */
void draw_addval_temp(int y)
{
	int update = 0;
	draw_st.new_temp = y;
	if (y <= draw_st.temp_min) { draw_st.temp_min = y; update = 1; draw_st.temp_min_set = 1; }
	if (y >= draw_st.temp_max) { draw_st.temp_max = y; update = 1; draw_st.temp_max_set = 1; }
	if (update) draw_print_temp_minmax();
}

/* add measured windspeed value to the screen
 */
void draw_addval_wind(int y)
{
	int update = 0;
	draw_st.new_windspeed = y;
	if (y <= draw_st.wind_min) { draw_st.wind_min = y; update = 1; draw_st.wind_min_set = 1; }
	if (y >= draw_st.wind_max) { draw_st.wind_max = y; update = 1; draw_st.wind_max_set = 1; }
	if (update) draw_print_wind_minmax();
}

/* update screen
 */
void draw_update(void)
{
	int x;
	int tval, wval;

	draw_constant();
	if (!draw_st.temp_min_set 
          ||!draw_st.temp_max_set
          ||!draw_st.wind_min_set
          ||!draw_st.wind_max_set) {
		return;
	}

	if (!draw_st.minmax_set) {
		draw_st.minmax_set = 1;
		draw_st.limit_temp_min = (draw_st.temp_min > 100) ? (draw_st.temp_min - 100) : 0;
		draw_st.old_limit_temp_min = draw_st.limit_temp_min;
		draw_st.limit_temp_max = draw_st.temp_max + 100;
		draw_st.old_limit_temp_max = draw_st.limit_temp_max;
		draw_st.limit_wind_min = (draw_st.wind_min > 100) ? (draw_st.wind_min - 100) : 0;
		draw_st.old_limit_wind_min = draw_st.limit_wind_min;
		draw_st.limit_wind_max = draw_st.wind_max + 100;
		draw_st.old_limit_wind_max = draw_st.limit_wind_max;

		draw_st.limit_temp_min = draw_st.old_limit_temp_min = 3400;
		draw_st.limit_temp_max = draw_st.old_limit_temp_max = 5000;
		draw_st.limit_wind_min = draw_st.old_limit_wind_min = 3400;
		draw_st.limit_wind_max = draw_st.old_limit_wind_max = 5000;

		memset((void *)&draw_st.temperature_table[0], draw_st.temp_min, sizeof(draw_st.temperature_table));
		memset((void *)&draw_st.windspeed_table[0], draw_st.wind_min, sizeof(draw_st.windspeed_table));
		return;
	}

	// first clean last drawing, both: temperature and windspeed
	for(x=0; x<DISPLAY_WINDOW_WIDTH; x++) {
		UInt32 xp, yp;
		UInt32 temp_y, wind_y;
		UInt32 temp_delta = draw_st.old_limit_temp_max - draw_st.old_limit_temp_min;
		UInt32 wind_delta = draw_st.old_limit_wind_max - draw_st.old_limit_wind_min;
		UInt32 display_height;

		display_height = ELEM_TEMP_HEIGHT;
		xp = x + ELEM_POS_TEMP_GRAPH_X;
		temp_y = draw_st.temperature_table[(x+draw_st.start_index) % DISPLAY_WINDOW_WIDTH];
		temp_y = (temp_y < draw_st.old_limit_temp_min) ? draw_st.old_limit_temp_min : temp_y;
		temp_y = (temp_y > draw_st.old_limit_temp_max) ? draw_st.old_limit_temp_max : temp_y;
		temp_y = display_height * (temp_y - draw_st.old_limit_temp_min) / temp_delta;
		yp = DISPLAY_WINDOW_HEIGHT - ELEM_POS_TEMP_GRAPH_Y - temp_y;
		WinErasePixel(xp, yp);

		display_height = ELEM_WIND_HEIGHT;
		xp = x + ELEM_POS_WIND_GRAPH_X;
		wind_y = draw_st.windspeed_table[(x+draw_st.start_index) % DISPLAY_WINDOW_WIDTH];
		wind_y = (wind_y < draw_st.old_limit_wind_min) ? draw_st.old_limit_wind_min : wind_y;
		wind_y = (wind_y > draw_st.old_limit_wind_max) ? draw_st.old_limit_wind_max : wind_y;
		wind_y = display_height * (wind_y - draw_st.old_limit_wind_min) / wind_delta;
		yp = DISPLAY_WINDOW_HEIGHT - ELEM_POS_WIND_GRAPH_Y - wind_y;
		WinErasePixel(xp, yp);
	}
	// clean also last numeric values
	WinEraseChars(draw_st.temperature_string, strlen(draw_st.temperature_string), ELEM_POS_TEMP_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_TEMP_NUM_Y);
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

		display_height = ELEM_TEMP_HEIGHT;
		xp = x + ELEM_POS_TEMP_GRAPH_X;
		temp_y = draw_st.temperature_table[(x+draw_st.start_index) % DISPLAY_WINDOW_WIDTH];
		temp_y = (temp_y < draw_st.limit_temp_min) ? draw_st.limit_temp_min : temp_y;
		temp_y = (temp_y > draw_st.limit_temp_max) ? draw_st.limit_temp_max : temp_y;
		temp_y = display_height * (temp_y - draw_st.limit_temp_min) / temp_delta;
		yp = DISPLAY_WINDOW_HEIGHT - ELEM_POS_TEMP_GRAPH_Y - temp_y;
		WinDrawPixel(xp, yp);

		display_height = ELEM_WIND_HEIGHT;
		xp = x + ELEM_POS_WIND_GRAPH_X;
		wind_y = draw_st.windspeed_table[(x+draw_st.start_index) % DISPLAY_WINDOW_WIDTH];
		wind_y = (wind_y < draw_st.limit_wind_min) ? draw_st.limit_wind_min : wind_y;
		wind_y = (wind_y > draw_st.limit_wind_max) ? draw_st.limit_wind_max : wind_y;
		wind_y = display_height * (wind_y - draw_st.limit_wind_min) / wind_delta;
		yp = DISPLAY_WINDOW_HEIGHT - ELEM_POS_WIND_GRAPH_Y - wind_y;
		WinDrawPixel(xp, yp);
	}
	// draw also new numeric values
	tval = draw_st.new_temp;
	wval = draw_st.new_windspeed;
	sprintf(draw_st.temperature_string, "%02d.%02d", tval / 100, tval % 100);
	sprintf(draw_st.windspeed_string,   "%02d.%02d", wval / 100, wval % 100);
	WinDrawChars(draw_st.temperature_string, strlen(draw_st.temperature_string), ELEM_POS_TEMP_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_TEMP_NUM_Y);
	WinDrawChars(draw_st.windspeed_string, strlen(draw_st.temperature_string), ELEM_POS_WIND_NUM_X, DISPLAY_WINDOW_HEIGHT - ELEM_POS_WIND_NUM_Y);
}

