//#include <Common.h>
//#include <System/SysAll.h>
//#include <UI/UIAll.h>
#include <PalmOS.h>
#include "storkmon.h"
#include "comm.h"
#include "draw.h"

#define LernAppID  'ReCh'
#define LernDBType 'Data'

FieldPtr f1;
FormPtr form;

char testtext[] = "Test 123";
int quit = 0;

static Boolean storkHandler(EventPtr event) {
	int handled = 0;

	switch(event->eType) {
		case frmOpenEvent:
			form = FrmGetActiveForm();
			f1 = FrmGetObjectPtr(form,
					FrmGetObjectIndex(form, fieldID_1));
/*			FldSetTextHandle(); */
			FldSetTextPtr(f1, testtext);
			FldRecalculateField(f1, true);
			FldDrawField(f1);

			FrmDrawForm(form);
			handled = 1;
			break;
		case ctlSelectEvent:
			switch(event->data.ctlEnter.controlID) {
				default:
					break;
			}
			handled = 1;
			break;
		case menuEvent:
			switch(event->data.menu.itemID) {
				case menuitemID_about:
					FrmAlert(alertID_about);
					break;
				case menuitemID_quit:
					quit = 1;
					break;

			}
			handled = 1;
			break;
		case nilEvent:
			handled = 1;
			break;
	}
	return handled;
}

static Boolean OpenDatabase(void) {
	return 0;
}

static int StartApplication(void) {
	int error;

	error = OpenDatabase();
	if(error) return error;

	/* initialize communication module */
	comm_init();

	/* initialize display module */
	draw_init();

	/* initialize ZigBee parser module */
	zbparser_init();

	FrmGotoForm(formID_stork);
}

static void EventLoop(void) {
	short err;
	int formID;
	FormPtr form;
	EventType event;
	int ret;
	int temp, windspeed;
	int timeout_counter = 0;
	int draw_enable = 0;

	do {
		//EvtGetEvent(&event, -1);
		EvtGetEvent(&event, 100);

		if(SysHandleEvent(&event)) continue;
		if(MenuHandleEvent((void *)0, &event, &err)) continue;

		if (timeout_counter == 0) {
			// perform action every timeout
			ret = comm_request_data();
			//ret = comm_request_info();
			if (ret != 0) {
				// error in communication - report it
				// TODO: report problem
			}
		}

		ret = comm_read_data(&temp, &windspeed);
		if (ret != 0) {
			// error in communication - report it
			// TODO: report problem
			temp = 0;
			windspeed = 0;
			draw_enable = 0;
		} else {
			//temp /= 100;
			//windspeed /= 100;
			if (temp > 9000) temp = 9000;
			if (windspeed > 9000) windspeed = 9000;
			draw_enable = 1;
		}
		
		//if (timeout_counter == 7 || draw_enable == 1) {
		if (draw_enable == 1) {
			// add points and draw them
			draw_addval_temp(temp);
			draw_addval_wind(windspeed);
			draw_update();
			draw_enable = 0;

			comm_reset_buffer();
		}

		if(event.eType == frmLoadEvent) {
			formID = event.data.frmLoad.formID;
			form = FrmInitForm(formID);
			FrmSetActiveForm(form);
			switch(formID) {
				case formID_stork:
					FrmSetEventHandler(form,
							(FormEventHandlerPtr) storkHandler);
					break;
			}
		}
		FrmDispatchEvent(&event);
		//timeout_counter = (timeout_counter+1)%2;
	} while(event.eType != appStopEvent && !quit);
}

static void StopApplication(void) {
	draw_destroy();
	comm_destroy();
}

UInt32  PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags) {
	int error;

	if(cmd == sysAppLaunchCmdNormalLaunch) {
		error = StartApplication();
		if(error) return error;
		
		EventLoop();
		StopApplication();
	}
	return 0;
}
