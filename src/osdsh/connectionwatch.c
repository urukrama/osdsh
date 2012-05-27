#include "osdsh.h"
#include "../config.h"
#define IAM 3
#define MYNAME "Net"

settings_t pppset;
void *connection_watch(void *arg);

char pppdevice[PATH_MAX+1];
char connecting1[BUFSIZ+1];
char connecting2[BUFSIZ+1];
char connected1[BUFSIZ+1];
char connected2[BUFSIZ+1];
char disconnected[BUFSIZ+1];
char timeconnected[255];

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
	set_defaults(&pppset);
}

/* This function takes a command and arguments, if the command is ours, execute
   it and return 1, if not, return 0*/

int isitmine( char command[BUFSIZ], char arg_first[BUFSIZ],
			char arg_secound[BUFSIZ])
{
	int i=1;

	if(strcmp(command, "stop")==0) {
		pppset.displaying = 0;
	}
	else if(strcmp(command, "start")==0) {
		if(!pppset.displaying) {
			pppset.displaying = 1;
			pthread_create(&pppset.mythread, NULL, connection_watch, NULL);
		}
	}
	else if (strcmp(command, "pppw")==0) {
		if (atoi(arg_first)) {
			isitmine("start", NULL, NULL);
		}
		else {
			pppset.displaying = 0;
		}
	}
	else if (strcmp(command, "sppp")==0) {
		control_options(&pppset, arg_first, arg_secound);
	}
	else if (strcmp(command, "pdev")==0) {
		sprintf(pppdevice, "%s", arg_first);
	}
	else if (strcmp(command, "connecting")==0) {
		sprintf(connecting1, "%s", arg_first);
		sprintf(connecting2, "%s", arg_secound);
	}
	else if (strcmp(command, "connected")==0) {
		sprintf(connected1, "%s", arg_first);
		sprintf(connected2, "%s", arg_secound);
	}
	else if (strcmp(command, "disconnected")==0) {
		sprintf(disconnected, "%s", arg_first);
	}
	else if (strcmp(command, "showconntime")==0) {
		xosd_display(pppset.myosd, 0, XOSD_string, "Connected for");
		xosd_display(pppset.myosd, 1, XOSD_string, timeconnected);
	}
	else {
		i = 0;
	}
	return i;
}


/*=============================connection_watch()=========================== */
/* some ideas from ppptime */
void *connection_watch(void *arg)
{
    FILE *fp;
    int ppp0run;

    long start;
    long end;
    int total;
    struct stat st;

    char ppp0pidfile[PATH_MAX];

    pppset.myosd = xosd_create(2);
    initialize_osd(&pppset);

    if(strcmp(pppdevice, "")==0)
	sprintf(pppdevice, "%s", PPP_DEVICE);

    sprintf(ppp0pidfile, "%s/%s.pid", PPP_PID_PATH, pppdevice);

    if (strcmp(connecting1, "")==0)
	sprintf(connecting1, "Connecting");
    if (strcmp(connecting2, "")==0)
	sprintf(connecting2, "please wait...");
	
    if (strcmp(connected1, "")==0)
	sprintf(connected1, "Connected");
    if (strcmp(connected2, "")==0)
	sprintf(connected2, "GO!");
	
    if (strcmp(disconnected, "")==0)
	sprintf(disconnected, "Connection terminated");

    while (pppset.displaying) {
	
	    xosd_display(pppset.myosd, 0, XOSD_string, connecting1);
	    xosd_display(pppset.myosd, 1, XOSD_string, connecting2);
	
	    sprintf(ppp0pidfile, "%s/%s.pid", PPP_PID_PATH, pppdevice);
	
	    if((fp=fopen(ppp0pidfile, "r"))==NULL) {
		ppp0run = 0;
	    }
	    else {
		ppp0run = 1;
		fclose(fp);
	    }
	
	    if(ppp0run) {
		xosd_display(pppset.myosd,0, XOSD_string, connected1);
		xosd_display(pppset.myosd,1, XOSD_string, connected2);
	
		stat(ppp0pidfile, &st);
		start = st.st_mtime; 
	
		while((fp=fopen(ppp0pidfile, "r"))!=NULL) {
		    fclose(fp);
		    end = time(0);
		    total = end - start;
		    sprintf(timeconnected, "Connected for %d hr %d min %d sec",
			((total/60)/60)%100, (total/60)%60, total%60);
		    usleep(3);
	   	}
		end = time(0);
		total = end - start;
		sprintf(timeconnected, "Disconnected after %d hr %d min %d sec",
			((total/60)/60)%100, (total/60)%60, total%60);
	
		xosd_display(pppset.myosd,0, XOSD_string, disconnected);
	   	xosd_display(pppset.myosd,1, XOSD_string, timeconnected);
	
		pppset.displaying = 0;
	    }
    }
    sleep(1);
    xosd_destroy(pppset.myosd);
    pthread_exit(0);
}
