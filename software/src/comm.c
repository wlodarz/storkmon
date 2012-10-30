#include <PalmOS.h>
#include <string.h>
#include "storkmon.h"
#include "comm.h"

#define MSG_MAX_SIZE 32
#define SERIAL_PORT_SPEED 19200

static const char *zigbee_bcast_msg = "AT+BCAST:00,";

struct comm_state {
	int initialized;
	unsigned short int portid;
};

static struct comm_state comm_st;

/* initialization of the communication module
 */
int comm_init(void)
{
	Err err;
	comm_st.initialized = 1;

	err = SrmOpen(serPortCradleRS232Port, SERIAL_PORT_SPEED, &comm_st.portid);
	ErrNonFatalDisplayIf(err == serErrBadPort, "serErrBadPort");

	switch(err) {
		case errNone:
			FrmAlert(alertID_serial1);
			break;
		case serErrAlreadyOpen:
			FrmAlert(alertID_serial2);
			break;
		default:
			FrmAlert(alertID_serial3);
			break;
	}
	
	SrmReceiveFlush(comm_st.portid, 100);
	
	return 0;
}

/* destroing of the communication module
 */
int comm_destroy(void)
{
	Err err;

	err = SrmSendWait(comm_st.portid);
	ErrNonFatalDisplayIf(err == serErrBadPort, "SrmClose: bad port");
	if (err == serErrTimeOut)
		FrmAlert(alertID_serial4);

	SrmClose(comm_st.portid);
	
	comm_st.initialized = 0;
	
	return 0;
}

static int comm_serial_send(const char *msg, int len)
{
	Err err;

	SrmSend(comm_st.portid, msg, len, &err);
	if (err) FrmAlert(alertID_serial5);
}

/* send message to serial, should be packet into AT+BCAST message
 */
static int comm_send_message(const char *msg)
{
	char buffer[MSG_MAX_SIZE];

	if (strlen(msg) > (MSG_MAX_SIZE - 1 - strlen(zigbee_bcast_msg))) return -1;

	sprintf(buffer, "%s%s\r", zigbee_bcast_msg, msg);
	comm_serial_send(buffer, strlen(buffer));

	return 0;
}

/* receive message from serial, unpack from BCAST message, and put into 'msg' buffer
 */
static int comm_recv_message(const char *msg, int maxlen)
{
}

/* request data from the remote node via communication module
 */
int comm_request_data(void)
{
	return comm_send_message("REQ_D");
}

/* read requested data into 'int *temp' and 'int *windspeed' variables
 */
int comm_read_data(int *temp, int *windspeed)
{
	*temp = SysRandom(TimGetTicks()) % 30;
	*windspeed = SysRandom(TimGetTicks()+1) % 30;
	return 0;
}

