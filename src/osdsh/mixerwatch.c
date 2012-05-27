#include "osdsh.h"
#include "../config.h"
#define IAM 0
#define MYNAME "Mixer"

char mixerdevice[PATH_MAX+1] = MIXER;
int dev;
settings_t mixerset;
int vol[SOUND_MIXER_NRDEVICES];
int old_vol[SOUND_MIXER_NRDEVICES];
char *devicelabels[SOUND_MIXER_NRDEVICES]=SOUND_DEVICE_LABELS;

void *mixer_watch(void *arg);

/* ==================== REQUIERED FOR PLUGINS ============================== 

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

/* This function sets the default (initial) values for our osd 
	Here i just load the default values, but you can load
	your own in your plugin.
*/
void initialize_myosd() {
	set_defaults(&mixerset);
}

/* This function takes a command and arguments, if the command is ours, execute
   it and return 1, if not, return 0*/

int isitmine( char command[BUFSIZ], char arg_first[BUFSIZ],
			char arg_secound[BUFSIZ])
{
	int i=1;

	if(strcmp(command, "stop")==0) {
		mixerset.displaying = 0;
	}
	else if(strcmp(command, "start")==0) {
		if(!mixerset.displaying) {
			mixerset.displaying = 1;
			pthread_create(&mixerset.mythread, NULL, mixer_watch, NULL);
		}
	}
	else if (strcmp(command, "smix")==0) {
		control_options(&mixerset, arg_first, arg_secound);
	}
	else if (strcmp(command, "mixr")==0) {
		if (atoi(arg_first)) {
			isitmine("start", NULL, NULL);
		}
		else {
			mixerset.displaying = 0;
		}
	}
	else {
		i = 0;
	}
	return i;
}




/*The next functions were taken from osdd-0.0.2,
written by W. Michael Petullo <osdd@flyn.org>, with some ideas, and slightly
(one or two chars) modified by me*/

/* ============================ initialize_vols () ========================= */
void initialize_vols( int devmask)
{
    int i;
    int volume;
    for (i=0;i<SOUND_MIXER_NRDEVICES;i++) {
	if (devmask & (1 << i)) {
	    if (ioctl(dev, MIXER_READ(i), &volume) == -1) {
            	perror(mixerdevice);
		mixerset.displaying=0;
	    }
	    old_vol[i] = vol[i] = volume;
	}
    }
}

/* ============================ display_channel () ========================= */
void display_channel(int channel)
{
    char channel_name[BUFSIZ];
    if (!channel) 
	sprintf(channel_name, "Master Volume");
    else 
	sprintf(channel_name, "%s Volume", devicelabels[channel]);

    if (ioctl(dev, MIXER_READ(channel), &vol[channel]) == -1) {
        perror(mixerdevice);
        mixerset.displaying = 0;
    }
    /*if muted, we say it!!*/
    if (!(vol[0] & 0xff || (vol[0] >> 8) & 0xff)) {
        xosd_display(mixerset.myosd, mixerset.position, XOSD_bottom, "Muted");
        xosd_display(mixerset.myosd, !mixerset.position, XOSD_bottom, "");
    }
    /* not muted, so lets check if the vol changed, if so, display
    them, if not, just ignore it*/
    else if (vol[channel] != old_vol[channel]) {
	xosd_display(mixerset.myosd, 0, XOSD_string, channel_name);
	xosd_display(mixerset.myosd, 1, XOSD_percentage,
		(((vol[channel]>>8)&0xff)+vol[channel]&0xff)/2);
    }
}

/*================================= mixer_watch() ============================*/
void *mixer_watch(void *arg)
{
    int devmask;
    int i;
    /* we open the osd for the mixer*/
    /* Mixer usually goes on the bottom of the screen... so*/

    mixerset.position = XOSD_bottom;

    mixerset.myosd = xosd_create(2);
    initialize_osd(&mixerset);

    /*open the mixer device for reading*/

    if ((dev = open(mixerdevice, O_RDWR)) < 0) {
	perror("Error opening");
        perror(mixerdevice);
        pthread_exit(0);
    }
    if (ioctl(dev, SOUND_MIXER_READ_DEVMASK, &devmask) == -1) {
	perror("Error reading");
        perror(mixerdevice);
        pthread_exit(0);
    }
    /*initialize the volume.....*/
    initialize_vols(devmask);

    while(mixerset.displaying) {
	    for (i=0;i<SOUND_MIXER_NRDEVICES;i++) {
		if (devmask & (1 << i)) {
		    display_channel(i);
		    old_vol[i] = vol[i];
		}
	    }
	    usleep(10000);
    }
    xosd_destroy(mixerset.myosd);
    pthread_exit(0);
}

