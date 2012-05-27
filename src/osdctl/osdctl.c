#include "osdctl.h"
#include "../config.h"

/* ============================ usage () =================================== */
void usage(const int exitcode, const char *error, const char *more)
{
    fprintf(stderr, "osdctl %s\n", VERSION);
    fprintf(stderr, "osdctl <command argument> ...\n\n");
    fprintf(stderr, "  -h                display help\n");
    fprintf(stderr, "  -s \"string\"       display a string\n");
    fprintf(stderr, "  -b (name,val)     display a bar up to val, named \"name\"\n");
    fprintf(stderr, "  -l (name,val)     display a slider at val, named \"name\"\n");
    fprintf(stderr, "  -t int            display the clock for int seconds\n");
    fprintf(stderr, "  -c <0|1>          (de)activate the osd clock\n");
    fprintf(stderr, "  -m <0|1>          (de)activate the mixer display\n");
    fprintf(stderr, "  -p <0|1>          (de)activate the ppp watch feature\n");
    fprintf(stderr, "  -a <0|1|2>        (de)activate the apm watch feature\n");
    fprintf(stderr, "  			 1=don't show percent bar; 2=do show it\n");
    fprintf(stderr, "  -d interface      set the ppp interface, default: ppp0\n");
    fprintf(stderr, "  -A int            show battery status for int seconds\n");
    fprintf(stderr, "  -e \"comm args\"    tell osdsh to do \"comm\" with \"args\"\n");
    fprintf(stderr, "  -S \"scriptfile\"   execute commands form scriptfile\n");
    fprintf(stderr, "  -x                tell osdsh to quit\n\n");
    if (error)
	fprintf(stderr, "%s: %s\n\n", error, more);
    exit(exitcode);
}

/* ============================= function prototypes ======================= */
void send_command(char command[BUFSIZ]);
void script_file(char file[PATH_MAX]);
int main(int argc, char *argv[]);

/*================================= send_command() ========================= */
void send_command(char command[BUFSIZ])
{
    FILE *fp;
    char fifo_file[PATH_MAX];

    if (strcmp(command, "")==0)
	return;

    sprintf(fifo_file, "%s.%d", OSD_FIFO_PATH, getuid());

    if((fp = fopen(fifo_file, "w")) == NULL) {
	perror("Couldn't open fifo");
	exit(1);
    }

    fputs(command, fp);

    fclose(fp);

}
/*============================= script_file() =========================*/
void script_file(char file[PATH_MAX])
{
    char readbuffer[BUFSIZ];
    FILE *fp;

    if((fp = fopen(file, "r")) == NULL) {
	perror("Couldn't find script file");
	exit(1);
    }
    while (fgets(readbuffer, BUFSIZ, fp)) {
	send_command(readbuffer);
    }
    fclose(fp);
}
/* ============================ main () ============================== */
int main(int argc, char *argv[])
{
    char c;
    char command[BUFSIZ];
    int opt_index = 0;
    struct option opts[] = {
	{"help", 0, 0, 'h'},
	{"string", 1, 0, 's'},
	{"bars", 1, 0, 'b'},
	{"slider", 1, 0, 'l'},
	{"flashtime", 1, 0, 't'},
	{"clock", 1, 0, 'c'},
	{"mixer", 1, 0, 'm'},
	{"execute", 1, 0, 'e'},
	{"exit", 0, 0, 'x'},
	{"script", 1, 0, 'S'},
	{"apm-watch", 1, 0, 'a'},
	{"apm-show", 1, 0, 'A'},
	{"ppp-watch", 1, 0, 'p'},
	{"ppp-dev", 1, 0, 'd'},
    };
    if (argc == 1)
	usage(0, NULL, NULL);
    while ((c = getopt_long(argc, argv, "hxs:b:l:t:c:m:e:S:p:d:a:A:", opts, &opt_index)) >= 0) {
	switch (c) {
	case 'h':
	    usage(0, NULL, NULL);
	    exit(0);
	case 's':
	    sprintf(command, "strn(%s)", optarg);
	    break;
	case 'b':
	    sprintf(command, "bars(%s)", optarg);
	    break;
	case 'l':
	    sprintf(command, "slid(%s)", optarg);
	    break;
	case 't':
	    sprintf(command, "time(%s)", optarg);
	    break;
	case 'c':
	    sprintf(command, "clck(%s)", optarg);
	    break;
	case 'm':
	    sprintf(command, "mixr(%s)", optarg);
	    break;
	case 'e':
	    sprintf(command, "%s", optarg);
	    break;
	case 'x':
	    sprintf(command, "exit()");
	    break;
	case 'S':
	    sprintf(command, "");
	    script_file(optarg);
	    break;
	case 'p':
	    sprintf(command, "pppw(%s)", optarg);
	    break;
	case 'd':
	    sprintf(command, "pdev(%s)", optarg);
	    break;
	case 'a':
	    sprintf(command, "apmw(%s)", optarg);
	    break;
	case 'A':
	    sprintf(command, "apms(%s)", optarg);
	    break;
	default:
	    usage(1, NULL, NULL);
	    exit(1);
	}
	send_command(command);
    }
    return(0);
}
