
#ifndef __STORKMON_COMM_H_
#define __STORKMON_COMM_H_

/* initialization of the communication module
 */
int comm_init(void);

/* destroing of the communication module
 */
int comm_destroy(void);

/* request data from the remote node via communication module
 */
int comm_request_data(void);

/* read requested data into 'int *temp' and 'int *windspeed' variables
 */
int comm_read_data(int *temp, int *windspeed);

#endif // __STORKMON_COMM_H_
