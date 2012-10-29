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

	FrmGotoForm(formID_stork);
}

static void EventLoop(void) {
	short err;
	int formID;
	FormPtr form;
	EventType event;
	int ret;
	int temp, windspeed;

	do {
		//EvtGetEvent(&event, -1);
		EvtGetEvent(&event, 50);

		if(SysHandleEvent(&event)) continue;
		if(MenuHandleEvent((void *)0, &event, &err)) continue;

		// perform action every timeout
		ret = comm_read_data(&temp, &windspeed);
		if (ret != 0) {
			// error in communication - report it
			// TODO: report problem
		} else {
			// add points and draw them
			draw_addval_temp(temp);
			draw_addval_wind(windspeed);
			draw_update();
		}
		ret = comm_request_data();
		if (ret != 0) {
			// error in communication - report it
			// TODO: report problem
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
