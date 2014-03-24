#ifndef __STORKMON_DRAW_H_
#define __STORKMON_DRAW_H_

/* Displayed elements position
 */

#define NUMERIC_VALUES_X_MARIGIN 23
#define NUMERIC_VALUES_Y_MARIGIN 0
#define NUMERIC_VALUE_HEIGHT 16
#define NUMERIC_VALUES_Y_TITLE_MARIGIN (NUMERIC_VALUE_HEIGHT * 0)
#define NUMERIC_VALUES_Y_MAX_MARIGIN (NUMERIC_VALUE_HEIGHT * 1)
#define NUMERIC_VALUES_Y_VAL_MARIGIN (NUMERIC_VALUE_HEIGHT * 2)
#define NUMERIC_VALUES_Y_MIN_MARIGIN (NUMERIC_VALUE_HEIGHT * 3)

/* Values
 */
#define DISPLAY_WINDOW_Y_OFFSET 20
#define NUM_VAL_LEN 8

#define SINGLE_WIND_STEP 100
#define SINGLE_TEMP_STEP 100

#define LIMIT_WIDTH 27
#define LIMIT_HEIGHT 15

#define LIMIT_WIND_MAX 1
#define LIMIT_WIND_MIN 2
#define LIMIT_TEMP_MAX 3
#define LIMIT_TEMP_MIN 4

#define LIMIT_TEMP_MAX_X (draw_st.screen_width - LIMIT_WIDTH - 1)
#define LIMIT_TEMP_MAX_Y (draw_st.y_marigin + NUMERIC_VALUES_Y_MAX_MARIGIN - 2)
#define LIMIT_TEMP_MIN_X (draw_st.screen_width - LIMIT_WIDTH - 1)
#define LIMIT_TEMP_MIN_Y (draw_st.y_marigin + NUMERIC_VALUES_Y_MIN_MARIGIN - 2)

#define LIMIT_WIND_MAX_X (draw_st.screen_width - LIMIT_WIDTH - 1)
#define LIMIT_WIND_MAX_Y (draw_st.y_marigin + (draw_st.screen_height / 2) + NUMERIC_VALUES_Y_MAX_MARIGIN - 2)
#define LIMIT_WIND_MIN_X (draw_st.screen_width - LIMIT_WIDTH - 1)
#define LIMIT_WIND_MIN_Y (draw_st.y_marigin + (draw_st.screen_height / 2) + NUMERIC_VALUES_Y_MIN_MARIGIN - 2)


#define FRAME_TEMP_X     (ELEM_POS_TEMP_GRAPH_X)
#define FRAME_TEMP_Y     (DISPLAY_WINDOW_HEIGHT - ELEM_POS_TEMP_GRAPH_Y)
#define FRAME_TEMP_WIDTH (DISPLAY_WINDOW_WIDTH)
#define FRAME_TEMP_HEIGHT (DISPLAY_WINDOW_HEIGHT / 2)

#define LIMIT_SELECT_TIMEOUT 6

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

void draw_select_limit(int limit);
void draw_deselect_limit(void);


#endif // __STORKMON_DRAW_H_
