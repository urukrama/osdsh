#include "osdsh.h"
#include "../config.h"


/* ==============================global variables=============================*/
/*
messages_t messages;
*/

/*basic setup here*/

extern settings_t settings;
/*
extern settings_t mixerset;
extern settings_t clockset;
extern settings_t pppset;
extern settings_t apmset;

void *apm_watch(void *arg);
void *control_sh(void *arg);
void *mixer_watch(void *arg);
void *connection_watch(void *arg);
void *clock_display(void *arg);
*/

/* ============================ usage () =================================== */
void usage(const int exitcode, const char *error, const char *more)
{
    fprintf(stderr, "osdsh %s\n", VERSION);
    fprintf(stderr, "usage: osdsh [options]\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  -h         display usage\n");
    fprintf(stderr, "  -m mixer   set mixer         %s\n", MIXER);
    fprintf(stderr, "  -f font    set font          %s \n", FONT);
    fprintf(stderr, "  -c color   set color         %s \n", COLOR);
    fprintf(stderr, "  -d int     set OSD delay     %d \n", TIMEOUT);
    fprintf(stderr, "  -o int     set Shadow Offset %d \n", SHADOW_OFFSET);
    fprintf(stderr, "  -p <0|1>   position of the osd, 0 for bottom,\n");
    fprintf(stderr, "             1 for top. Default 1 (top).\n");
    fprintf(stderr, "  -a <0|1|2> align of the osd. left,center or right.\n");
    fprintf(stderr, "  -n number  set the nice number so osdsh won't eat your cpu\n");
    fprintf(stderr, "\n");
    if (error)
	fprintf(stderr, "%s: %s\n\n", error, more);
    exit(exitcode);
}



/* ============================ parse_args () ============================== */
void parse_args(int argc, char *argv[], settings_t * settings)
{
    char c;
    int opt_index = 0;
    struct option opts[] = {
	{"help", 0, 0, 'h'},
	{"position", 1, 0, 'p'},
	{"soffset", 1, 0, 'o'},
	{"mixer", 1, 0, 'm'},
	{"font", 1, 0, 'f'},
	{"color", 1, 0, 'c'},
	{"delay", 1, 0, 'd'},
	{"align", 1, 0, 'a'},
	{"nice", 1, 0, 'n'}
    };
    while ((c = getopt_long(argc, argv, "hm:f:c:p:d:o:n:", opts, &opt_index)) >= 0) {
	switch (c) {
	case 'h':
	    usage(0, NULL, NULL);
	    exit(0);
	case 'f':
	    strncpy(settings->font, optarg, BUFSIZ);
	    settings->font[BUFSIZ] = '\0';
	    break;
	case 'c':
	    strncpy(settings->color, optarg, BUFSIZ);
	    settings->color[BUFSIZ] = '\0';
	    break;
	case 'p':
		settings->position = atoi(optarg);
	    break;
	case 'a':
	    settings->align = atoi(optarg);
	    break;
	case 'd':
	    settings->delay = atoi(optarg);
	    break;
/*
	case 'm':
	    strncpy(messages.mixerdevice, optarg, BUFSIZ);
	    messages.mixerdevice[PATH_MAX+1] = '\0';
	    break;
*/
	case 'o':
	    settings->soffset = atoi(optarg);
	    break;
	case 'n':
	    nice(atoi(optarg));
	    break;
	default:
	    usage(1, NULL, NULL);
	    exit(1);
	}
    }
}

void load_basic_plugins(void) {
	int a;
	char file[PATH_MAX+255];

	sprintf(file, "%s/libosdshmixer.so", PLUGINSDIR);
	a = load_plugin(file);
	if (a<0) {
		xosd_display(settings.myosd, 0, XOSD_string, "No Mixer Support");
		xosd_display(settings.myosd, 1, XOSD_string, "");
	}

	sprintf(file, "%s/libosdshclock.so", PLUGINSDIR);
	a = load_plugin(file);
	if (a<0) {
		xosd_display(settings.myosd, 0, XOSD_string, "No Clock Support");
		xosd_display(settings.myosd, 1, XOSD_string, "");
	}

	sprintf(file, "%s/libosdshnet.so", PLUGINSDIR);
	a = load_plugin(file);
	if (a<0) {
		xosd_display(settings.myosd, 0, XOSD_string, "No Net Support");
		xosd_display(settings.myosd, 1, XOSD_string, "");
	}

	sprintf(file, "%s/libosdshapm.so", PLUGINSDIR);
	a = load_plugin(file);
	if (a<0) {
		xosd_display(settings.myosd, 0, XOSD_string, "No APM/Battery support");
		xosd_display(settings.myosd, 1, XOSD_string, "");
	}
}

/* ============================ main () ==================================== */

int main(int argc, char *argv[], char *env[])
{
    pid_t childpid;

    char pid_file[PATH_MAX+1];
    FILE *fp;

    set_defaults(&settings);
    initialize_osd(&settings);

    parse_args(argc, argv, &settings);
/*
#ifndef NOAPM
    apmset = settings;
#endif
    pppset = mixerset = settings;
*/

    if((childpid=fork())<0) {
	perror("fork");
	exit(1);
    }
    if (childpid==0) {
	    control_sh(NULL);
    }
    else {
	sprintf(pid_file, "%s.%d.pid", OSD_FIFO_PATH, getuid()) ;
	fp=fopen(pid_file, "w");
	fprintf(fp, "%d", childpid);
	fclose(fp);
	exit(0);
    }	
    return(0);
}
