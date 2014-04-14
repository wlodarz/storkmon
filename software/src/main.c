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
int autooff_counter;

static Boolean storkHandler(EventPtr event) {
	int handled = 0;
	char msg[10];

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
		case penDownEvent:
			handled = 1;
			FrmAlert(alertID_pendown);
			break;
		case nilEvent:
			handled = 1;
			break;
	}
	return handled;
}

static Boolean hardwareButtonsHandler(EventPtr event) {
	int handled = 0;

	if(event->eType == keyDownEvent) {
		switch(event->data.keyDown.chr) {
			case vchrPageUp:
				handled = 1;
				//FrmAlert(alertID_up);
				draw_inc_limit();
				break;
			case vchrPageDown:
				handled = 1;
				draw_dec_limit();
				break;
			case vchrHard1:
				draw_select_limit(LIMIT_TEMP_MAX);
				handled = 1;
				break;
			case vchrHard2:
				draw_select_limit(LIMIT_TEMP_MIN);
				handled = 1;
				break;
			case vchrHard3:
				draw_select_limit(LIMIT_WIND_MAX);
				handled = 1;
				break;
			case vchrHard4:
				draw_select_limit(LIMIT_WIND_MIN);
				handled = 1;
				break;
			case vchrAutoOff:
				autooff_counter = 0;
				break;
		}
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
	static UInt32 lastResetTime;
	
	lastResetTime = TimGetSeconds();
	autooff_counter = 0;
	do {
		//EvtGetEvent(&event, -1);
		EvtGetEvent(&event, 50);

		if (TimGetSeconds() - lastResetTime > 30) {
			if (autooff_counter < ((40-1) * 2)) EvtResetAutoOffTimer();
			autooff_counter++;
			lastResetTime = TimGetSeconds();
		}

		if(hardwareButtonsHandler(&event)) continue;
		if(SysHandleEvent(&event)) continue;
		if(MenuHandleEvent((void *)0, &event, &err)) continue;

		if (event.eType == nilEvent) {
			// perform action every timeout
			ret = comm_request_data();
			//ret = comm_request_info();
			if (ret != 0) {
				// error in communication - report it
				// TODO: report problem
			}

			ret = comm_read_data(&temp, &windspeed);
			if (ret != 0) {
				// error in communication - report it
				// TODO: report problem
				temp = 0;
				windspeed = 0;
				draw_enable = 0;
			} else {
				if (temp > 9000) temp = 9000;
				if (windspeed > 9000) windspeed = 9000;
				draw_enable = 1;
			}

			draw_deselect_limit();
		}
		
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
