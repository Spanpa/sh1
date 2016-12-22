/*  ECAM 2016
    Othman MEJDOUBI & Zakariya CHIHI
    myrm : remove files or directories
    usage : myrm [option]... [file]...
    3 options :
        -i, --interactive  prompt before every removal
        -h, --help         display the help
        -v, --verbose      explain what is being done */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>

#define YELLOW      "\x1b[33m"
#define BOLD        "\x1b[1m"
#define RESET       "\x1b[0m"

int main (int argc, char **argv) {

int c, unlk;
char *rm_value = NULL;
int do_help = 0;
int do_interactive = 0;
int do_verbose = 0;
int invalid_option = 0;

struct option longopts [] = {
	{ "interactive", optional_argument , NULL , 'i' },
	{ "help", no_argument , NULL , 'h' },
    { "verbose", optional_argument , NULL , 'v' },
	{ 0, 0, 0, 0 }
};

/* Check if a file given its pathname is a regular file */
int is_regular_file(char *path){
    struct stat path_stat;
    // stats the file pointed to by path and fills in path_stat
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

void rm(void){
    if(rm_value != NULL){
        if (is_regular_file(rm_value)){
            // deletes the file give its pathname
            unlk = unlink(rm_value);
            if(unlk == 0){
                // keep going
            }
            else {
                printf("myrm: cannot remove '%s': No such file", rm_value);
            }
        }
        else {
            printf("myrm: cannot remove '%s': No such file or Is not a regular file\n", rm_value);
        }
    }
    else {
        printf("myrm: missing operand\n");
        printf("Try 'myrm --help' for more information.\n");
    }
}

void rm_verbose(void){
    if(rm_value != NULL){
        if (is_regular_file(rm_value)){
            // deletes the file given its pathname
            unlk = unlink(rm_value);
            if(unlk == 0){
                // displays the name of the file that was removed
                printf("removed '%s'\n", rm_value);
            }
            else {
                printf("myrm: cannot remove '%s': No such file", rm_value);
            }
        }
        else {
            printf("myrm: cannot remove '%s': No such file or Is not a regular file\n", rm_value);
        }
    }
    else {
        printf("myrm: missing operand\n");
        printf("Try 'myrm --help' for more information.\n");
    }
}

void rm_interactive(void){
    if(rm_value != NULL){
        if (is_regular_file(rm_value)){
            char confirm;
            // asks the user to confirm before deleting the file
            printf("myrm: remove regular file '%s'? (y/n) ", rm_value);
            scanf(" %c", &confirm);
            if(confirm == 'y'){            
                // deletes the file given its pathname
                unlk = unlink(rm_value);
                if(unlk == 0){
                    // keep going
                }
                else {
                    printf("myrm: cannot remove '%s': No such file", rm_value);
                }            
            }
        }
        else {
            printf("myrm: cannot remove '%s': No such file or Is not a regular file\n", rm_value);
        }
    }
    else {
        printf("myrm: missing operand\n");
        printf("Try 'myrm --help' for more information.\n");
    }
}

/* Function that prints the help */
void help(void){
    printf(BOLD "USAGE\n" RESET);
    printf("    myrm [option]... [file]...\n\n");
    printf(BOLD "DESCRIPTION\n" RESET);
    printf("    remove files.\n\n");
    printf(BOLD "    -v, --verbose " RESET);
    printf("explain what is being done\n\n");
    printf(BOLD "    -h, --help " RESET);
    printf("display this help and exit\n\n");
    printf(BOLD "    -i,  " RESET);
    printf("prompt before every removal\n\n");    
    printf("    Written by ");
    printf(BOLD YELLOW"Othman MEJDOUBI " RESET);
    printf("& ");
    printf(BOLD YELLOW"Zakaria CHIHI" RESET);
    printf("\n\n");
}

while ((c = getopt_long (argc, argv, "hiv", longopts, NULL)) != -1) {
	switch (c) {
    case 'h':
        do_help = 1;
        break ;
	case 'i':
		do_interactive = 1;
		rm_value = argv[2];
		break ;
    case 'v':
        do_verbose = 1;
        rm_value = argv[2];
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
    else if(do_verbose == 1 && do_interactive == 0){
        rm_verbose();
    }
    else if(do_verbose == 0 && do_interactive == 1){
        rm_interactive();
    }
    else if(do_verbose == 0 && do_interactive == 0){
        rm_value = argv[1];
        rm();
    }
}

}