
#include <PalmOS.h>
#include <stdio.h>
#include <string.h>
#include "draw.h"

//#define DISPLAY_WINDOW_WIDTH 120
#define MAX_VALUE_INDEX 1024
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

	UInt32 new_limit_temp_min, new_limit_temp_max;
	UInt32 new_limit_wind_min, new_limit_wind_max;
	UInt32 old_limit_temp_min, old_limit_temp_max;
	UInt32 old_limit_wind_min, old_limit_wind_max;

	UInt32 temp_max, wind_max;

	UInt32 temp_table[MAX_VALUE_INDEX];
	UInt32 wind_table[MAX_VALUE_INDEX];

	int constant_drawed;

	int limit_selected;
	int limit_selected_timeout;
	RectangleType selection_rect;
	int limit;

	int x_marigin, y_marigin;
	int screen_width, screen_height;

	int graph_x_marigin, graph_y_marigin;
	int graph_width, graph_height;

	int max_value_index;

	int new_time_scale, old_time_scale;
};

static struct draw_state draw_st;

/* init draw module
 */
int draw_init(void)
{
	UInt32 xd, yd;

	draw_st.start_index = 0;
	draw_st.constant_drawed = 0;
	draw_st.first_wind_set = 0;
	draw_st.first_temp_set = 0;
	memset((void *)&draw_st.temp_table[0], 0, sizeof(draw_st.temp_table));
	memset((void *)&draw_st.wind_table[0], 0, sizeof(draw_st.wind_table));
	*(char *)draw_st.temperature_string = '\0';
	*(char *)draw_st.windspeed_string = '\0';
	draw_st.limit_selected = 0;
	draw_st.limit_selected_timeout = 0;

	WinScreenMode(winScreenModeGet, &xd, &yd, NULL, NULL);
	draw_st.screen_width = xd;
	draw_st.screen_height = yd - DISPLAY_WINDOW_Y_OFFSET;
	draw_st.x_marigin = 0;
	draw_st.y_marigin = DISPLAY_WINDOW_Y_OFFSET;

	draw_st.graph_x_marigin = draw_st.x_marigin;
	draw_st.graph_y_marigin = draw_st.y_marigin;
	draw_st.graph_width = draw_st.screen_width - NUMERIC_VALUES_X_MARIGIN - 4;
	draw_st.graph_height = draw_st.screen_height;

	draw_st.max_value_index = draw_st.graph_width;

	draw_st.temp_max = draw_st.wind_max = 0;

	draw_st.new_windspeed = draw_st.new_temp = 0;

	draw_st.new_time_scale =  draw_st.old_time_scale = 1;

	draw_st.limit = LIMIT_TIME;

	return 0;
}

int draw_constant(void)
{
        char *wind_str = "WIND";
        char *temp_str = "TEMP";

	WinDrawChars(temp_str, strlen(temp_str), draw_st.x_marigin + draw_st.screen_width - NUMERIC_VALUES_X_MARIGIN, draw_st.y_marigin + NUMERIC_VALUES_Y_TITLE_MARIGIN);
	WinDrawChars(wind_str, strlen(wind_str), draw_st.x_marigin + draw_st.screen_width - NUMERIC_VALUES_X_MARIGIN, draw_st.y_marigin + (draw_st.screen_height / 2) + NUMERIC_VALUES_Y_TITLE_MARIGIN);

	WinDrawLine(draw_st.x_marigin + 0,  draw_st.y_marigin + 0, 
	            draw_st.x_marigin + draw_st.graph_width,  draw_st.y_marigin + 0);
	WinDrawLine(draw_st.x_marigin + draw_st.graph_width,  draw_st.y_marigin + 0, 
	            draw_st.x_marigin + draw_st.graph_width,  draw_st.y_marigin + draw_st.graph_height-1);
	WinDrawLine(draw_st.x_marigin + draw_st.graph_width,  draw_st.y_marigin + draw_st.graph_height-1, 
	            draw_st.x_marigin + 0,  draw_st.y_marigin + draw_st.graph_height-1);
	WinDrawLine(draw_st.x_marigin + 0,  draw_st.y_marigin + draw_st.graph_height-1, 
	            draw_st.x_marigin + 0,  draw_st.y_marigin + 0);
	WinDrawLine(draw_st.x_marigin + 0,  draw_st.y_marigin + (draw_st.graph_height / 2), 
	            draw_st.x_marigin + draw_st.graph_width,  draw_st.y_marigin + (draw_st.graph_height / 2));


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
	WinEraseChars(draw_st.wind_limit_max_string, strlen(draw_st.wind_limit_max_string), 
		draw_st.x_marigin + draw_st.screen_width        - NUMERIC_VALUES_X_MARIGIN, 
		draw_st.y_marigin + (draw_st.screen_height / 2) + NUMERIC_VALUES_Y_MARIGIN + NUMERIC_VALUES_Y_MAX_MARIGIN);
	WinEraseChars(draw_st.wind_limit_min_string, strlen(draw_st.wind_limit_min_string), 
		draw_st.x_marigin + draw_st.screen_width        - NUMERIC_VALUES_X_MARIGIN, 
		draw_st.y_marigin + (draw_st.screen_height / 2) + NUMERIC_VALUES_Y_MARIGIN + NUMERIC_VALUES_Y_MIN_MARIGIN);

	val = draw_st.new_limit_wind_min;
	sprintf(draw_st.wind_limit_min_string, "%02d.%02d", val / SINGLE_WIND_DIVIDER, val % SINGLE_WIND_DIVIDER);
	val = draw_st.new_limit_wind_max;
	sprintf(draw_st.wind_limit_max_string, "%02d.%02d", val / SINGLE_WIND_DIVIDER, val % SINGLE_WIND_DIVIDER);

	WinDrawChars(draw_st.wind_limit_max_string, strlen(draw_st.wind_limit_max_string), 
		draw_st.x_marigin + draw_st.screen_width        - NUMERIC_VALUES_X_MARIGIN, 
		draw_st.y_marigin + (draw_st.screen_height / 2) + NUMERIC_VALUES_Y_MAX_MARIGIN  + NUMERIC_VALUES_Y_MARIGIN);
	WinDrawChars(draw_st.wind_limit_min_string, strlen(draw_st.wind_limit_min_string), 
		draw_st.x_marigin + draw_st.screen_width        - NUMERIC_VALUES_X_MARIGIN, 
		draw_st.y_marigin + (draw_st.screen_height / 2) + NUMERIC_VALUES_Y_MIN_MARIGIN  + NUMERIC_VALUES_Y_MARIGIN);
}

void draw_print_temp_limits(void)
{
	int val;

	WinEraseChars(draw_st.temp_limit_min_string, strlen(draw_st.temp_limit_min_string), 
		draw_st.x_marigin + draw_st.screen_width        - NUMERIC_VALUES_X_MARIGIN, 
		draw_st.y_marigin + NUMERIC_VALUES_Y_MIN_MARIGIN  + NUMERIC_VALUES_Y_MARIGIN);
	WinEraseChars(draw_st.temp_limit_max_string, strlen(draw_st.temp_limit_max_string), 
		draw_st.x_marigin + draw_st.screen_width        - NUMERIC_VALUES_X_MARIGIN, 
		draw_st.y_marigin + NUMERIC_VALUES_Y_MAX_MARIGIN  + NUMERIC_VALUES_Y_MARIGIN);

	val = draw_st.new_limit_temp_min;
	sprintf(draw_st.temp_limit_min_string, "%02d.%02d", val / SINGLE_TEMP_DIVIDER, val % SINGLE_TEMP_DIVIDER);
	val = draw_st.new_limit_temp_max;
	sprintf(draw_st.temp_limit_max_string, "%02d.%02d", val / SINGLE_TEMP_DIVIDER, val % SINGLE_TEMP_DIVIDER);

	WinDrawChars(draw_st.temp_limit_min_string, strlen(draw_st.temp_limit_min_string), 
		draw_st.x_marigin + draw_st.screen_width        - NUMERIC_VALUES_X_MARIGIN, 
		draw_st.y_marigin + NUMERIC_VALUES_Y_MIN_MARIGIN  + NUMERIC_VALUES_Y_MARIGIN);
	WinDrawChars(draw_st.temp_limit_max_string, strlen(draw_st.temp_limit_max_string), 
		draw_st.x_marigin + draw_st.screen_width        - NUMERIC_VALUES_X_MARIGIN, 
		draw_st.y_marigin + NUMERIC_VALUES_Y_MAX_MARIGIN  + NUMERIC_VALUES_Y_MARIGIN);
}

void draw_print_temp_max(void)
{
	char msg[10];
	int val = draw_st.temp_max;
	sprintf(msg, "T:%02d.%02d", val / SINGLE_TEMP_DIVIDER, val % SINGLE_TEMP_DIVIDER);
	WinDrawChars(msg, strlen(msg), draw_st.x_marigin + 92, 1);
}

void draw_print_wind_max(void)
{
	char msg[10];
	int val = draw_st.wind_max;
	sprintf(msg, "W:%02d.%02d", val / SINGLE_WIND_DIVIDER, val % SINGLE_WIND_DIVIDER);
	WinDrawChars(msg, strlen(msg), draw_st.x_marigin + 127, 1);
}

/* add measured temperature value to the screen
 */
void draw_addval_temp(int y)
{
	draw_st.new_temp = y;
	if (y >= draw_st.temp_max) {
		draw_st.temp_max = y;
		draw_print_temp_max();
	}
}

/* add measured windspeed value to the screen
 */
void draw_addval_wind(int y)
{
	draw_st.new_windspeed = y;
	if (y >= draw_st.wind_max) {
		draw_st.wind_max = y;
		draw_print_wind_max();
	}
}

/* update screen
 */
void draw_update(void)
{
	int x;
	int tval, wval;
	UInt16 current_seconds;
	UInt16 x_range_marigin = 9;
	UInt16 x_range = 25;
	UInt16 seconds_per_range = 20;
	int x_data_scale = 1;
	int max_timelines = 6;
	char msg[30];
	int i;
	UInt32 temp_delta;
	UInt32 wind_delta;
	UInt32 display_height = (draw_st.graph_height - 4) / 2;

	draw_constant();

	if (!draw_st.first_temp_set && !draw_st.first_wind_set) {
		RectangleType rect;
		UInt16 cornerDiam = 0;

		rect.topLeft.x = 0;
		rect.topLeft.y = 0;
		rect.extent.x = 160;
		rect.extent.y = 20;
		WinEraseRectangle(&rect, cornerDiam);
	}

	if (!draw_st.first_temp_set) {
		UInt32 new_temp;

		new_temp = (draw_st.new_temp / SINGLE_TEMP_STEP) * SINGLE_TEMP_STEP;
		draw_st.new_limit_temp_min = (new_temp >= SINGLE_TEMP_STEP) ? (new_temp - SINGLE_TEMP_STEP) : 0;
		draw_st.old_limit_temp_min = draw_st.new_limit_temp_min;
		draw_st.new_limit_temp_max = new_temp + SINGLE_TEMP_STEP * 2;
		draw_st.old_limit_temp_max = draw_st.new_limit_temp_max;
		memset((void *)&draw_st.temp_table[0], draw_st.new_temp, sizeof(draw_st.temp_table));
		draw_print_temp_limits();
		draw_st.first_temp_set = 1;
	}

	if (!draw_st.first_wind_set) {
		draw_st.new_limit_wind_min = 0;
		draw_st.old_limit_wind_min = draw_st.new_limit_wind_min;
		draw_st.new_limit_wind_max = 1500;
		draw_st.old_limit_wind_max = draw_st.new_limit_wind_max;
		memset((void *)&draw_st.wind_table[0], draw_st.new_windspeed, sizeof(draw_st.wind_table));
		draw_print_wind_limits();
		draw_st.first_wind_set = 1;
	}

#define COMPUTE_PIXEL(type, y_low, new_old) \
	{ \
		UInt32 val = 0;											\
		int i; \
		for(i=0; i<draw_st. ## new_old ## _time_scale; i++) { \
			val += (draw_st. ## type ## _table[(draw_st.start_index  - (draw_st.max_value_index - x - 1) * draw_st. ## new_old ## _time_scale - i) % MAX_VALUE_INDEX]);   		\
		} \
		val /= draw_st. ## new_old ## _time_scale; \
		val = (val < draw_st. ## new_old ## _limit_ ## type ## _min) ? draw_st. ## new_old ## _limit_ ## type ## _min : val; 			\
		val = (val > draw_st. ## new_old ## _limit_ ## type ## _max) ? draw_st. ## new_old ## _limit_ ## type ## _max : val;			\
		val = display_height * (val - draw_st. ## new_old ## _limit_ ## type ## _min) / type ## _delta;				\
		yp = draw_st.graph_y_marigin + y_low - val; 							\
	}
	
	temp_delta = draw_st.old_limit_temp_max - draw_st.old_limit_temp_min;
	wind_delta = draw_st.old_limit_wind_max - draw_st.old_limit_wind_min;

	// first clean last drawing, both: temperature and windspeed
	for(x=0; x<draw_st.max_value_index; x++) {
		UInt32 xp = x + draw_st.graph_x_marigin, yp;

		if (draw_st.first_temp_set) {
			display_height = (draw_st.graph_height - 4) / 2;
			COMPUTE_PIXEL(temp, ((draw_st.graph_height / 2) - 1), old)
			WinErasePixel(xp, yp);
		}

		if (draw_st.first_wind_set) {
			display_height -= 1;
			COMPUTE_PIXEL(wind, ((draw_st.graph_height) - 2), old)
			WinErasePixel(xp, yp);
		}
	}

	// clean also last numeric values
	if (draw_st.first_temp_set)
		WinEraseChars(draw_st.temperature_string, strlen(draw_st.temperature_string), 
				draw_st.x_marigin + draw_st.screen_width        - NUMERIC_VALUES_X_MARIGIN, 
				draw_st.y_marigin + NUMERIC_VALUES_Y_MARIGIN + NUMERIC_VALUES_Y_VAL_MARIGIN);
	if (draw_st.first_wind_set)
		WinEraseChars(draw_st.windspeed_string, strlen(draw_st.temperature_string), 
				draw_st.x_marigin + draw_st.screen_width        - NUMERIC_VALUES_X_MARIGIN, 
				draw_st.y_marigin + (draw_st.screen_height / 2 ) + NUMERIC_VALUES_Y_MARIGIN + NUMERIC_VALUES_Y_VAL_MARIGIN);

	// add new values
	//draw_st.temp_table[(x+draw_st.start_index)%draw_st.max_value_index] = draw_st.new_temp;
	//draw_st.wind_table[(x+draw_st.start_index)%draw_st.max_value_index] = draw_st.new_windspeed;
	draw_st.temp_table[(draw_st.start_index)%MAX_VALUE_INDEX] = draw_st.new_temp;
	draw_st.wind_table[(draw_st.start_index)%MAX_VALUE_INDEX] = draw_st.new_windspeed;

	// timelines and time values
	for(i=0; i<max_timelines; i++) {
		sprintf(msg, "%03d", (i) * seconds_per_range * draw_st.new_time_scale);
		WinDrawChars(msg, strlen(msg), draw_st.x_marigin + draw_st.graph_width - (i*x_range) - 4, 10);

		WinDrawLine(draw_st.x_marigin + draw_st.graph_width - (i*x_range),  draw_st.y_marigin, 
	            	draw_st.x_marigin + draw_st.graph_width - (i*x_range),  draw_st.y_marigin + draw_st.screen_height);
	}

	// move start index
	draw_st.start_index = (draw_st.start_index + 1) % MAX_VALUE_INDEX; // draw_st.max_value_index;

	temp_delta = draw_st.new_limit_temp_max - draw_st.new_limit_temp_min;
	wind_delta = draw_st.new_limit_wind_max - draw_st.new_limit_wind_min;

	// now draw new drawing, both: temperature and windspeed
	for(x=0; x<draw_st.max_value_index; x++) {
		UInt32 xp = x + draw_st.graph_x_marigin, yp;
		UInt32 display_height;

		if (draw_st.first_temp_set) {
			display_height = (draw_st.graph_height - 4) / 2;
			COMPUTE_PIXEL(temp, ((draw_st.graph_height / 2) - 1), new)
			WinDrawPixel(xp, yp);
			// old = current
			draw_st.old_limit_temp_min = draw_st.new_limit_temp_min;
			draw_st.old_limit_temp_max = draw_st.new_limit_temp_max;
		}

		if (draw_st.first_wind_set) {
			display_height -= 1;
			COMPUTE_PIXEL(wind, (draw_st.graph_height - 2), new)
			WinDrawPixel(xp, yp);
			// old = current
			draw_st.old_limit_wind_min = draw_st.new_limit_wind_min;
			draw_st.old_limit_wind_max = draw_st.new_limit_wind_max;
		}
		if (draw_st.first_wind_set || draw_st.first_temp_set)
			draw_st.old_time_scale = draw_st.new_time_scale;
	}

	// draw also new numeric values
	tval = draw_st.new_temp;
	wval = draw_st.new_windspeed;
	sprintf(draw_st.temperature_string, "%02d.%02d", tval / SINGLE_TEMP_DIVIDER, tval % SINGLE_TEMP_DIVIDER);
	sprintf(draw_st.windspeed_string,   "%02d.%02d", wval / SINGLE_WIND_DIVIDER, wval % SINGLE_WIND_DIVIDER);
	if (draw_st.first_temp_set)
		WinDrawChars(draw_st.temperature_string, strlen(draw_st.temperature_string),
				draw_st.x_marigin + draw_st.screen_width        - NUMERIC_VALUES_X_MARIGIN, 
				draw_st.y_marigin + NUMERIC_VALUES_Y_MARIGIN + NUMERIC_VALUES_Y_VAL_MARIGIN);
	if (draw_st.first_wind_set)
		WinDrawChars(draw_st.windspeed_string, strlen(draw_st.temperature_string),
				draw_st.x_marigin + draw_st.screen_width        - NUMERIC_VALUES_X_MARIGIN, 
				draw_st.y_marigin + (draw_st.screen_height / 2 ) + NUMERIC_VALUES_Y_MARIGIN + NUMERIC_VALUES_Y_VAL_MARIGIN);
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
			draw_st.selection_rect.extent.x  = LIMIT_WIDTH;
			draw_st.selection_rect.extent.y  = LIMIT_HEIGHT;
			break;
		case LIMIT_WIND_MIN:
			draw_st.selection_rect.topLeft.x = LIMIT_WIND_MIN_X;
			draw_st.selection_rect.topLeft.y = LIMIT_WIND_MIN_Y;
			draw_st.selection_rect.extent.x  = LIMIT_WIDTH;
			draw_st.selection_rect.extent.y  = LIMIT_HEIGHT;
			break;
		case LIMIT_TEMP_MAX:
			draw_st.selection_rect.topLeft.x = LIMIT_TEMP_MAX_X;
			draw_st.selection_rect.topLeft.y = LIMIT_TEMP_MAX_Y;
			draw_st.selection_rect.extent.x  = LIMIT_WIDTH;
			draw_st.selection_rect.extent.y  = LIMIT_HEIGHT;
			break;
		case LIMIT_TEMP_MIN:
			draw_st.selection_rect.topLeft.x = LIMIT_TEMP_MIN_X;
			draw_st.selection_rect.topLeft.y = LIMIT_TEMP_MIN_Y;
			draw_st.selection_rect.extent.x  = LIMIT_WIDTH;
			draw_st.selection_rect.extent.y  = LIMIT_HEIGHT;
			break;
		default:
			return;
	}
	/* select limit by drawing frame */
	draw_st.limit_selected = 1;
	draw_st.limit_selected_timeout = LIMIT_SELECT_TIMEOUT;

	x = draw_st.selection_rect.topLeft.x;
	y = draw_st.selection_rect.topLeft.y;
	dx = draw_st.selection_rect.extent.x;
	dy = draw_st.selection_rect.extent.y;
	draw_paint_selection(x, y, dx, dy);
}

void draw_inc_limit(void)
{
	switch(draw_st.limit) {
		case LIMIT_WIND_MAX:
			if (draw_st.new_limit_wind_max < 9900) draw_st.new_limit_wind_max += SINGLE_WIND_STEP;
			break;
		case LIMIT_WIND_MIN:
			if ((draw_st.new_limit_wind_min + SINGLE_WIND_STEP) < draw_st.new_limit_wind_max) draw_st.new_limit_wind_min += SINGLE_WIND_STEP;
			break;
		case LIMIT_TEMP_MAX:
			if (draw_st.new_limit_temp_max < 9900) draw_st.new_limit_temp_max += SINGLE_TEMP_STEP;
			break;
		case LIMIT_TEMP_MIN:
			if ((draw_st.new_limit_temp_min + SINGLE_TEMP_STEP) < draw_st.new_limit_temp_max) draw_st.new_limit_temp_min += SINGLE_TEMP_STEP;
		default:
			if (draw_st.new_time_scale < 4) draw_st.new_time_scale++;
			break;
	}
	draw_st.limit_selected_timeout = LIMIT_SELECT_TIMEOUT;
	draw_print_temp_limits();
	draw_print_wind_limits();
}

void draw_dec_limit(void)
{
	switch(draw_st.limit) {
		case LIMIT_WIND_MAX:
			if ((draw_st.new_limit_wind_max-SINGLE_WIND_STEP) > draw_st.new_limit_wind_min) draw_st.new_limit_wind_max -= SINGLE_WIND_STEP;
			break;
		case LIMIT_WIND_MIN:
			if (draw_st.new_limit_wind_min >= SINGLE_WIND_STEP) draw_st.new_limit_wind_min -= SINGLE_WIND_STEP;
			break;
		case LIMIT_TEMP_MAX:
			if ((draw_st.new_limit_temp_max-SINGLE_TEMP_STEP) > draw_st.new_limit_temp_min) draw_st.new_limit_temp_max -= SINGLE_TEMP_STEP;
			break;
		case LIMIT_TEMP_MIN:
			if (draw_st.new_limit_temp_min >= SINGLE_TEMP_STEP) draw_st.new_limit_temp_min -= SINGLE_TEMP_STEP;
		default:
			if (draw_st.new_time_scale > 1) draw_st.new_time_scale--;
			break;
	}
	draw_st.limit_selected_timeout = LIMIT_SELECT_TIMEOUT;
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

	draw_st.limit = LIMIT_TIME;
}
