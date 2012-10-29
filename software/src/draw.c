
#include <PalmOS.h>
#include <stdio.h>
#include <string.h>
#include "draw.h"

#define DISPLAY_WINDOW_WIDTH 120
#define NUM_VAL_LEN 8

struct draw_state {
	int new_temp; // recently added temperature value
	int new_windspeed; // recently added windspeed value

	int start_index; // what is starting point

	signed char temperature_string[NUM_VAL_LEN];
	signed char windspeed_string[NUM_VAL_LEN];
	char temperature_table[DISPLAY_WINDOW_WIDTH];
	char windspeed_table[DISPLAY_WINDOW_WIDTH];
};

static struct draw_state draw_st;

/* init draw module
 */
int draw_init(void)
{
	draw_st.start_index = 0;
	memset((void *)&draw_st.temperature_table[0], 0, sizeof(draw_st.temperature_table));
	memset((void *)&draw_st.windspeed_table[0], 0, sizeof(draw_st.windspeed_table));
	*(char *)draw_st.temperature_string = '\0';
	*(char *)draw_st.windspeed_string = '\0';

	return 0;
}

/* destroy draw module
 */
int draw_destroy(void)
{
	return 0;
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
	// first clean last drawing, both: temperature and windspeed
	for(x=0; x<DISPLAY_WINDOW_WIDTH; x++) {
		WinErasePixel(x+ELEM_POS_TEMP_GRAPH_X, 
			     draw_st.temperature_table[(x+draw_st.start_index)%DISPLAY_WINDOW_WIDTH] + ELEM_POS_TEMP_GRAPH_Y);
		WinErasePixel(x+ELEM_POS_WIND_GRAPH_X, 
			     draw_st.windspeed_table[(x+draw_st.start_index)%DISPLAY_WINDOW_WIDTH] + ELEM_POS_WIND_GRAPH_Y);
	}
	// clean also last numeric values
	WinEraseChars(draw_st.temperature_string, strlen(draw_st.temperature_string), ELEM_POS_TEMP_NUM_X, ELEM_POS_TEMP_NUM_Y);
	WinEraseChars(draw_st.windspeed_string, strlen(draw_st.temperature_string), ELEM_POS_WIND_NUM_X, ELEM_POS_WIND_NUM_Y);

	// add new values
	draw_st.temperature_table[(x+draw_st.start_index)%DISPLAY_WINDOW_WIDTH] = draw_st.new_temp;
	draw_st.windspeed_table[(x+draw_st.start_index)%DISPLAY_WINDOW_WIDTH] = draw_st.new_windspeed;

	// move start index
	draw_st.start_index = (draw_st.start_index + 1) % DISPLAY_WINDOW_WIDTH;

	// now draw new drawing, both: temperature and windspeed
	for(x=0; x<DISPLAY_WINDOW_WIDTH; x++) {
		WinDrawPixel(x+ELEM_POS_TEMP_GRAPH_X, 
			     draw_st.temperature_table[(x+draw_st.start_index)%DISPLAY_WINDOW_WIDTH] + ELEM_POS_TEMP_GRAPH_Y);
		WinDrawPixel(x+ELEM_POS_WIND_GRAPH_X, 
			     draw_st.windspeed_table[(x+draw_st.start_index)%DISPLAY_WINDOW_WIDTH] + ELEM_POS_WIND_GRAPH_Y);
	}
	// draw also new numeric values
	sprintf(draw_st.temperature_string, "%03d", draw_st.new_temp);
	sprintf(draw_st.windspeed_string, "%03d", draw_st.new_windspeed);
	WinDrawChars(draw_st.temperature_string, strlen(draw_st.temperature_string), ELEM_POS_TEMP_NUM_X, ELEM_POS_TEMP_NUM_Y);
	WinDrawChars(draw_st.windspeed_string, strlen(draw_st.temperature_string), ELEM_POS_WIND_NUM_X, ELEM_POS_WIND_NUM_Y);
}

