#include "osdsh.h"
#include "../config.h"

    int n=0;
    plugininfo_t plugins[MAX_PLUGINS];
    settings_t settings;

/* ================================== load a plugin ============================= */
int load_plugin( char pluginfile[255])
{
    int (*info)();
    void *module;
    int a;

    char file[PATH_MAX+256];
    char *msg = NULL;


/*    getcwd(file, PATH_MAX);
    strcat(file, "/");
*/
    strcat(file, pluginfile);

    module = dlopen(pluginfile, RTLD_NOW);

    if(!module) {
	msg = dlerror();
	if(msg != NULL) {
		fprintf(stderr, "%s\n", msg);
		a = -1;
	}
    }
    else {
	info = dlsym(module, "whoami");
	a = info();

	info = dlsym(module, "initialize_myosd");
	info();

	plugins[a].isitmine = dlsym(module, "isitmine");

	info = dlsym(module, "mynameis");

	plugins[a].module = module;
	plugins[a].whoami = a;
	plugins[a].file = file;
	plugins[a].myname = (char *) info();
	plugins[a].active = 1;
    }
    return a;
}
 /* =========================== close a plugin ===============================*/

/*
int close_plugin( int a)
{
    char report[255];

    if(dlclose(plugins[a].module)) {
	sprintf(report, "Error while unloading %s", plugins[a].myname);
	plugins[a].displaying = 1;
	return -1;
    }
    else {
	sprintf(report, "Successfully unloaded %s", plugins[a].myname);
	plugins[a].displaying = 0;
	return 0;
    }
    xosd_display(settings.myosd, 0, XOSD_string, report);
    xosd_display(settings.myosd, 1, XOSD_string, "");
}
*/


/*============================ display_stuff()=============================== */
void display_stuff(char command[BUFSIZ], char arguments[BUFSIZ])
{
    char c;
    char arg_first[BUFSIZ];
    char arg_secound[BUFSIZ];
    int a,i;

    sprintf(arg_first, "");
    sprintf(arg_secound, "");

    /* separate the arguments in sub_arguments .... or not?*/
    i=0;
    a=0;
    while ((c=arguments[i])!=','&&c!='\n'&&c!='\0') {
	if (c=='\\') {
	    i++;
	    arg_first[a] = arguments[i];
	}
	else {
	    arg_first[a] = c;
	}
	a++;
	i++;
    }
    arg_first[a] = '\0';
    i++;
    a = 0;

    while((c=arguments[i])!='\n'&&c!='\0'&&c!=')') {
	if (c=='\\') {
	    i++;
	    arg_secound[a] = arguments[i];
	}
        else {
	    arg_secound[a] = c;
	}
	a++;
	i++;
    }
    arg_secound[a] = '\0';

    /* done with the arguments */
 
    if (strcmp(command, "load")==0) {

/*
	for(i=0;i<n+1;i++) {
		if(strcmp(arg_first, plugins[i].myname)==0) {
			strcpy(arg_secound, plugins[i].file);
		}
	}

	a = load_plugin( arg_secound);

*/	a = load_plugin( arg_first );
	sprintf(arg_secound, "Error loading %s", arg_first);

	if(a>=0) {
		if(a>n&&a<MAX_PLUGINS)
			n=a;
		if(plugins[a].isitmine("stop", NULL, NULL)) {
			sprintf(arg_secound, "Successfully loaded %s", plugins[a].myname);
		}
	}

	xosd_display(settings.myosd, 0, XOSD_string, arg_secound);
	xosd_display(settings.myosd, 1, XOSD_string, "");

    }
    else if (strcmp(command, "strn")==0) {
	xosd_display(settings.myosd, 0, XOSD_string, arg_first);
	xosd_display(settings.myosd, 1, XOSD_string, arg_secound);
    }
    else if (strcmp(command, "bars")==0) {
	xosd_display(settings.myosd, 0, XOSD_string, arg_first);
	xosd_display(settings.myosd, 1, XOSD_percentage, atoi(arg_secound));
    }
    else if (strcmp(command, "slid")==0) {
	xosd_display(settings.myosd, 0, XOSD_string, arg_first);
	xosd_display(settings.myosd, 1, XOSD_slider, atoi(arg_secound));
    }
    else if (strcmp(command, "dset")==0) {
	control_options(&settings, arg_first, arg_secound);
    }
    else if (strcmp(command, "exit")==0) {
	xosd_display(settings.myosd, 0, XOSD_string, "Goodbye");
	xosd_display(settings.myosd, 1, XOSD_string, "");
	for(i=0;i<n+1;i++) {
		if(plugins[i].active) {
			plugins[i].isitmine("stop", NULL, NULL);
		}
	}
	sleep(2);
    	settings.displaying = 0;
    }
    else {
	i = 0;
	a = 0;
	while(i==0&&a<MAX_PLUGINS) {
		if(plugins[a].active) {
			i = plugins[a].isitmine(command, arg_first, arg_secound);
		}
		a++;
	}
	if(i==0) {
		xosd_display(settings.myosd, 0, XOSD_string, command);
		xosd_display(settings.myosd, 1, XOSD_string, "No such command");
	}
    }
}

/*================================control_sh() ============================= */
void *control_sh(void *arg)
{
    int a,i;
    char c;
    char command[BUFSIZ];
    char command_arg[BUFSIZ];
    
    char fifo_file[PATH_MAX +1];

    FILE *fp;
    char readbuf[BUFSIZ];

    sprintf(fifo_file, "%s.%d",OSD_FIFO_PATH, getuid());
    settings.displaying = 1;
    /* we open the osd*/
	settings.myosd = xosd_create (2);

	initialize_osd(&settings);
	load_basic_plugins();

    /* create the fifo file*/
	unlink(fifo_file);
	umask(0);
	mknod(fifo_file, S_IFIFO|0600, 0);
	    /*open and read the fifo*/
	while (settings.displaying) {
		fp = fopen(fifo_file, "r");
		/*Read the bufer and check that it's not a comment*/
		while(fgets(readbuf, BUFSIZ, fp)) {

		    if (readbuf[c]=='#') continue;

		    /*ignore spaces*/
		    for (i=0; i<BUFSIZ&&((c=readbuf[i])==' '||c=='\t'); i++);
		    /*get the command*/
		    for (a=0; (c=readbuf[i])!='\n'&&c!='('; a++,i++)
			command[a] = readbuf[i];
		    command[a] = '\0';
		    /*get the command's arguments*/
		    i ++;
		    for(a=0; (c=readbuf[i])!='\0'&&c!='\n'&&c!=')';a++,i++) {
			if (c=='\\') {
			    i++;
			    command_arg[a] = readbuf[i];
		        }
			else {
			    command_arg[a]=c;
			}
		    }
		    command_arg[a] = '\0';
		    /*execute the command from the fifo and close it*/
		    display_stuff(command, command_arg);
		}
		fclose(fp);
	}
	unlink(fifo_file);
	sprintf(fifo_file, "%s.pid", fifo_file);
	unlink(fifo_file);
	xosd_destroy(settings.myosd);
	exit(0);
}
