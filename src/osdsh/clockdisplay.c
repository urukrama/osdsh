#include "../config.h"
#include "osdsh.h"
#define IAM 1
#define MYNAME "Clock"

char clockformat[255] = CLOCK_FORMAT;

void *osdsh_clock(void *arg);
settings_t clocksetts;

/*================================clock_display() ========================== */
/* got the ideas from osd_clock */


/* ==========================================================================
   ====================== REQUIERED STUFF FOR PLUGINS =======================
   ==========================================================================

***** ID of the plugin, contact me before you asign this to your plugin!! *****
*/
int whoami(void) {
	return IAM;
}

/* Return a pointer to the name of the plugin */
char *mynameis() {
	char *a = MYNAME;
	return a;
}

/* This function sets the default (initual) values for our osd */
void initialize_myosd() {
	set_defaults(&clocksetts);
}

/* This function takes a command and arguments, if the command is ours, execute
   it and return 1, if not, return 0*/

int isitmine( char command[BUFSIZ], char arg_first[BUFSIZ],
			char arg_secound[BUFSIZ])
{
	int i=1;

	if(strcmp(command, "stop")==0) {
		clocksetts.displaying = 0;
	}
	else if(strcmp(command, "start")==0) {
		if(!clocksetts.displaying) {
			clocksetts.displaying = 1;
			pthread_create(&clocksetts.mythread, NULL, osdsh_clock, NULL);
		}
	}
	else if (strcmp(command, "clkf")==0) {
		strcpy(clockformat ,arg_first);
	}
	else if (strcmp(command, "clck")==0) {
		if (atoi(arg_first)) {
			isitmine("start", NULL, NULL);
		}
		else {
		    clocksetts.displaying = 0;
		}
	}
	else if (strcmp(command, "time")==0) {
		if (atoi(arg_first)) 
			isitmine("start", NULL, NULL);
		sleep(atoi(arg_first));
		clocksetts.displaying = 0;
	}
	else if (strcmp(command, "sclk")==0) {
		control_options(&clocksetts, arg_first, arg_secound);
	}
	else {
		i = 0;
	}
	return i;
}

/*The functtion to call when we start our plugin... this sould be called by a
pthread(...) call so it won't block the whole program*/

void *osdsh_clock(void *arg)
{
    time_t currenttime;
    char timestring[255];

    clocksetts.myosd  = xosd_create(1);
    initialize_osd(&clocksetts);
    clocksetts.displaying = 1;

    while (clocksetts.displaying) {
	    currenttime = time(0);

	    strftime(timestring, 255, clockformat, localtime(&currenttime));

	    xosd_display(clocksetts.myosd, 0, XOSD_string, timestring);
    }
    xosd_destroy(clocksetts.myosd);
    pthread_exit(0);
}
