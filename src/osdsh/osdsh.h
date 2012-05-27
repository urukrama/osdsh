#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <pthread.h>
#include <dlfcn.h>

#include <linux/limits.h>
#include <linux/stat.h>
#include <linux/soundcard.h>

#include <xosd.h>

/*============================== settings_t ==================================*/

typedef struct settings_t {
    xosd *myosd;
    char font[BUFSIZ + 1];
    char color[BUFSIZ + 1];
    int soffset;
    int xoffset;
    int yoffset;
    xosd_align align;
    xosd_pos position;
    int displaying;
    int delay;
    pthread_t mythread;
} settings_t;

typedef struct plugininfo_t {
    int whoami;
    int active;
    void *module;
    char *file;
    char *myname;
    int (*isitmine)();
} plugininfo_t;

