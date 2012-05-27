#include "osdsh.h"
#include "../config.h"
#include <apm.h>
#define IAM 2
#define MYNAME "APM / Battery"

settings_t apmset;
void *apm_watch(void *arg);

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
	set_defaults(&apmset);
}

/* This function takes a command and arguments, if the command is ours, execute
   it and return 1, if not, return 0*/

int isitmine( char command[BUFSIZ], char arg_first[BUFSIZ],
			char arg_secound[BUFSIZ])
{
	int i=1;

	if(strcmp(command, "stop")==0) {
		apmset.displaying = 0;
	}
	else if(strcmp(command, "start")==0) {
		if(!apmset.displaying) {
			apmset.displaying = 1;
			pthread_create(&apmset.mythread, NULL, apm_watch, NULL);
		}
	}
	else if (strcmp(command, "apmw")==0) {
		if (atoi(arg_first)) {
			isitmine("start", NULL, NULL);
		}
		else {
			apmset.displaying = 0;
		}
	}
	else if (strcmp(command, "apms")==0) {
		isitmine("start", NULL, NULL);
		sleep(atoi(arg_first));
		apmset.displaying = 0;
	}
	else if (strcmp(command, "sapm")==0) {
		control_options(&apmset, arg_first, arg_secound);
	}
	else {
		i = 0;
	}
	return i;
}

/*=============================apm_watch()=========================== */
/* some ideas (shamelessly copied ) from apmd's "apm" utility*/
void *apm_watch(void *arg)
{
    int secs;
    apm_info i;
    char battery[255];
    char remainingtime[255];
    char report1[255];

    apmset.myosd = xosd_create(2);
    initialize_osd(&apmset);

    while(apmset.displaying) {
	    if (apm_read(&i))
	    {
		/*fprintf(stderr, "Cannot read APM information\n");*/
		apmset.displaying = 0;
		xosd_display(apmset.myosd, apmset.position==XOSD_bottom?1:0, XOSD_string,
			"Cannot read APM information");
		break;
	    }
	    secs = i.using_minutes ? i.battery_time * 60 : i.battery_time;
            if (i.battery_flags != 0xff	/* Have a 1.1 BIOS and a system battery. ?????? */
		&& i.battery_flags & 0x80) {
		sprintf(battery, "no system battery");
	    }
	    else {
		switch (i.battery_status) {
		case 0:
	    	    sprintf(battery, "high");
		    xosd_set_colour(apmset.myosd, apmset.color);
	    	break;
		case 1:
	    	    sprintf(battery, "low");
		    xosd_set_colour(apmset.myosd, "yellow");
	    	break;
		case 2:
	    	    sprintf(battery, "critical!!");
		    xosd_set_colour(apmset.myosd, "red");
	    	break;
		case 3:
	    	    sprintf(battery, "charging...");
	    	break;
	        }

	    }
	    if (strcmp(battery, "no system battery")==0) {
		xosd_display(apmset.myosd, apmset.position==XOSD_bottom?1:0, XOSD_string,
			"AC on line");
		xosd_display(apmset.myosd, apmset.position==XOSD_bottom?0:1, XOSD_string, battery);
	    }
	    else {
		sprintf(remainingtime, "%dhr, %dmin, %dsec",
				secs/3600,
				(secs%3600)/60,
				((secs%3600)%60)%60);
		sprintf(report1, "AC: %s Remaining battery: %d%%(%s) %s",
			i.ac_line_status?"on line":"off line",i.battery_percentage,
			remainingtime, battery);

		xosd_display(apmset.myosd, 0, XOSD_string, report1);
		if (apmset.displaying > 1)
			xosd_display(apmset.myosd, 1, XOSD_percentage, i.battery_percentage );
		else 
			xosd_display(apmset.myosd, 1, XOSD_string, "");
	    }
    }
    pthread_exit(0);
}
