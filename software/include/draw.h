#ifndef __STORKMON_DRAW_H_
#define __STORKMON_DRAW_H_

/* Displayed elements position
 */
/* temperature numeric value position */
#define ELEM_POS_TEMP_NUM_X 140
#define ELEM_POS_TEMP_NUM_Y 20
/* windspeed numeric value position */
#define ELEM_POS_WIND_NUM_X 140
#define ELEM_POS_WIND_NUM_Y 80
/* temperature graph position */
#define ELEM_POS_TEMP_GRAPH_X 10
#define ELEM_POS_TEMP_GRAPH_Y 20
/* windspeed graph position */
#define ELEM_POS_WIND_GRAPH_X 10
#define ELEM_POS_WIND_GRAPH_Y 80

/* Values
 */
#define DISPLAY_WINDOW_HEIGHT 160
#define DISPLAY_WINDOW_WIDTH 120
#define NUM_VAL_LEN 8


/* init draw module
 */
int draw_init(void);

/* destroy draw module
 */
int draw_destroy(void);

/* add measured temperature value to the screen
 */
void draw_addval_temp(int y);

/* add measured windspeed value to the screen
 */
void draw_addval_wind(int y);

/* update screen
 */
void draw_update(void);




#endif // __STORKMON_DRAW_H_
