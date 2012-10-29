
#include <PalmOS.h>
#include "comm.h"

struct comm_state {
	int initialized;
};

static struct comm_state comm_st;

/* initialization of the communication module
 */
int comm_init(void)
{
	comm_st.initialized = 1;
	return 0;
}

/* destroing of the communication module
 */
int comm_destroy(void)
{
	comm_st.initialized = 0;
	return 0;
}

/* request data from the remote node via communication module
 */
int comm_request_data(void)
{
	return 0;
}

/* read requested data into 'int *temp' and 'int *windspeed' variables
 */
int comm_read_data(int *temp, int *windspeed)
{
	*temp = SysRandom(TimGetTicks()) % 30;
	*windspeed = SysRandom(TimGetTicks()+1) % 30;
	return 0;
}

