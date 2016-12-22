/*  ECAM 2016
    Othman MEJDOUBI & Zakaria CHIHI
    mypwd : print name of current/working directory.
    usage : mypwd [option]
    1 option :
        -h, --help       display the help */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <getopt.h>

#define YELLOW      "\x1b[33m"
#define BOLD        "\x1b[1m"
#define RESET       "\x1b[0m"

int main(int argc, char **argv) {

int c;
int do_help = 0;
int invalid_option = 0;

struct option longopts [] = {
	{ "help", no_argument , NULL , 'h' },
	{ 0, 0, 0, 0 }
};

void pwd(void){
    char *working_dir;
    char buffer[PATH_MAX + 1];
    /* getcwd() returns an absolute pathname that is the current working directory
       of the calling process. The pathname is returned as the function result 
       and via the argument buf, if present. */
    working_dir = getcwd(buffer, PATH_MAX + 1);
    if(working_dir != NULL) {
        printf("%s\n", working_dir);
    }
}

/* Function that prints the help */
void help(void){
    printf(BOLD "DESCRIPTION\n" RESET);
    printf("    Print the full filename of the current working directory.\n\n");
    printf(BOLD "    -h, --help " RESET);
    printf("display this help and exit\n\n");
    printf(BOLD "AUTHORS\n" RESET);
    printf("    Written by ");
    printf(BOLD YELLOW"Othman MEJDOUBI " RESET);
    printf("& ");
    printf(BOLD YELLOW"Zakaria CHIHI\n" RESET);
}

while ((c = getopt_long (argc, argv, "h", longopts, NULL)) != -1) {
	switch (c) {
	case 'h':
		do_help = 1;
		break ;
	case 0: /* getopt_long () set a variable , just keep going */
		break ;
	case '?':
	default : /* invalid option */
		fprintf (stderr, "%s: option ‘-%c’ is invalid : ignored, try --help to get the command syntax.  \n",
		argv[1], optopt);
		invalid_option = 1;
		break ;
	case -1:
		break;
	}
}

if(invalid_option == 0){
	if(do_help == 1){
    	help();
	}
	else{
    	pwd();
	}
}

}