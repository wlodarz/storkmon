#include <PalmOS.h>
#include <stdio.h>
#include <string.h>
#include "storkmon.h"
#include "comm.h"

#define MSG_MAX_SIZE 32
#define SERIAL_PORT_SPEED 19200

static const char *zigbee_bcast_msg = "AT+BCAST:00,";

struct comm_state {
	int initialized;
	unsigned short int portid;
	unsigned int msglen;
	char message[MSG_MAX_SIZE];
};

static struct comm_state comm_st;

/* initialization of the communication module
 */
int comm_init(void)
{
	Err err;
	comm_st.initialized = 1;
	comm_st.message[0] = '\0';
	comm_st.msglen = 0;

	err = SrmOpen(serPortCradleRS232Port, SERIAL_PORT_SPEED, &comm_st.portid);
	ErrNonFatalDisplayIf(err == serErrBadPort, "serErrBadPort");

	switch(err) {
		case errNone:
			//FrmAlert(alertID_serial1);
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

	if (StrLen(msg) > (MSG_MAX_SIZE - 1 - StrLen(zigbee_bcast_msg))) return -1;

	//sprintf(buffer, "%s%s\r", zigbee_bcast_msg, msg);
	StrCopy(buffer, zigbee_bcast_msg);
	StrCat(buffer, msg);
	comm_serial_send(buffer, StrLen(buffer));

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
	return comm_send_message("REQ_D\r\n");
}

/* read requested data into 'int *temp' and 'int *windspeed' variables
 */
int comm_read_data(int *temp, int *windspeed)
{
	int eol=0;
	int timeout = 4, i, len;
	Err error;

	*temp = 15;
	*windspeed = 15;
	/* try to read one line */
	i=0;
	while(comm_st.msglen < MSG_MAX_SIZE && !eol && timeout > 0) {
		SrmReceive(comm_st.portid, &comm_st.message[comm_st.msglen], 1, 5, &error);
		if (!error) {
			if (comm_st.message[comm_st.msglen] == chrCarriageReturn || comm_st.message[comm_st.msglen] == chrLineFeed) { 
				eol = 1; comm_st.message[comm_st.msglen] = '\0';
		}
			comm_st.msglen++;
		} else {
			timeout--;
			SrmReceiveFlush(comm_st.portid, 1);
		}
	}


	/* check if timeout occured */
	if (timeout > 0) {

		/* find temperature string */
		char *tstr = StrStr(comm_st.message, "temp");
		char *wstr = StrStr(comm_st.message, "wind");
		*temp = 10;
		*windspeed = 11;
		if (tstr && wstr) {
			*temp = (tstr[5] - '0')  * 1000 + (tstr[6] - '0') * 100 + (tstr[8] - '0') * 10 + (tstr[9] - '0');
			*windspeed = (wstr[5] - '0') * 1000 + (wstr[6] - '0') * 100 + (wstr[8] - '0') * 10 + (wstr[9] - '0');
			//FrmCustomAlert(alertID_debug, comm_st.message, NULL, NULL);
		}
	} else {
		*temp = 13;
		*windspeed = 14;
		return -1;
	}

	return 0;
}

/* check if new temperature and wind speed available
 */
int comm_samples_available(void)
{
	return 0;
}

void comm_reset_buffer(void)
{
	comm_st.message[0] = '\0';
	comm_st.msglen = 0;
}

