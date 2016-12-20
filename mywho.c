/*  ECAM 2016
	Othman MEJDOUBI & Zakariya CHIHI
	mywho : Print information about users who are currently logged in.
	usage : mywho [option]
	8 options(including 5 additional) :
    	-a, --all 		print all processes
    	-b, --boot 		time of last system boot
    	-d, --dead 		print dead processes
    	-h, --help  	display the help
    	-l, --login  	print system login processes
    	-q, --count  	all login names and number of users logged on
    	-r, --runlevel  print current runlevel
    	-u, --users  	list users logged in */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <utmp.h>

#define YELLOW      "\x1b[33m"
#define BOLD        "\x1b[1m"
#define RESET       "\x1b[0m"

int main (int argc, char **argv) {

int c;
int do_all = 0;
int do_boot = 0;
int do_dead = 0;
int do_help = 0;
int do_login = 0;
int do_count = 0;
int do_runlevel = 0;
int do_users = 0;
int invalid_option = 0;

int runlevel;
time_t time;
char time_buff[100];
const int logsize = 16;
int fd;
struct stat fileStat;

// open utmp file and get the file descriptor
fd = open("/var/run/utmp", O_RDONLY);
struct utmp log[logsize];
// read the file with fd and store its contents into an array of struct utmp
read(fd, &log, logsize * sizeof (struct utmp));

struct option longopts [] = {
	{ "all", no_argument , NULL , 'a' },
	{ "boot", no_argument , NULL , 'b' },
	{ "dead", no_argument , NULL , 'd' },
	{ "help", no_argument , NULL , 'h' },
	{ "login", no_argument , NULL , 'l' },
	{ "count", no_argument , NULL , 'q' },
	{ "runlevel", no_argument , NULL , 'r' },
	{ "users", no_argument , NULL , 'u' },
	{ 0, 0, 0, 0 }
};

// Prints only login and user processes
void who(void){
	for(int i = 0; i < logsize; i++){
		if((&log[i])->ut_type == 6 || (&log[i])->ut_type == 7){
			time = (&log[i])->ut_tv.tv_sec;
			strftime(time_buff, 100, "%F %H:%M", localtime(&time));
			printf("%s   %s         %s\n", (&log[i])->ut_user, (&log[i])->ut_line, time_buff);
		}
	} 
}

// Prints time of system boot, runlevel, init, login user and dead processes
void who_a(void){
	runlevel = 0;
	for(int i = 0; i < logsize; i++){
		time = (&log[i])->ut_tv.tv_sec;
		strftime(time_buff, 100, "%F %H:%M", localtime(&time));
		// time of system boot 2 (in ut_tv)
		if((&log[i])->ut_type == 2){
			printf("         system boot  %s\n", time_buff);
		}
		// actual system runlevel
		if((&log[i])->ut_type == 1 && runlevel == 0){
			printf("         %s %c   %s\n", (&log[i])->ut_user, (&log[i])->ut_pid % 256, time_buff);
			runlevel++;
		}
		// init, login, user and dead process
		if((&log[i])->ut_type == 5 || (&log[i])->ut_type == 6 || (&log[i])->ut_type == 7 || (&log[i])->ut_type == 8){
			printf("%s   %s         %s         %d\n", (&log[i])->ut_user, (&log[i])->ut_line, time_buff, (&log[i])->ut_pid);
		}
	} 
}

// Time of system boot 2 (in ut_tv)
void who_b(void){
	for(int i = 0; i < logsize; i++){
		if((&log[i])->ut_type == 2){
			time = (&log[i])->ut_tv.tv_sec;
			strftime(time_buff, 100, "%F %H:%M", localtime(&time));
			printf("         system boot  %s\n", time_buff);
		}
	} 
}

// Terminated process 8
void who_d(void){
	for(int i = 0; i < logsize; i++){
		if((&log[i])->ut_type == 8){
			time = (&log[i])->ut_tv.tv_sec;
			strftime(time_buff, 100, "%F %H:%M", localtime(&time));
			printf("%s   %s         %s         %d\n", (&log[i])->ut_user, (&log[i])->ut_line, time_buff, (&log[i])->ut_pid);
		}
	} 
}

// Session leader process for user login 6
void who_l(void){
	for(int i = 0; i < logsize; i++){
		if((&log[i])->ut_type == 6){
			time = (&log[i])->ut_tv.tv_sec;
			strftime(time_buff, 100, "%F %H:%M", localtime(&time));
			printf("%s   %s         %s         %d\n", (&log[i])->ut_user, (&log[i])->ut_line, time_buff, (&log[i])->ut_pid);
		}
	} 
}

// Normal user processes 7 and count
void who_q(void){
	int count = 0;
	for(int i = 0; i < logsize; i++){
		if((&log[i])->ut_type == 7){
			printf("%s ", (&log[i])->ut_user);
			count++;
		}
	}
	// print number of normal users
	printf("\n# users=%d\n", count);
}

// Actual system runlevel
void who_r(void){
	runlevel = 0;
	for(int i = 0; i < logsize; i++){
		time = (&log[i])->ut_tv.tv_sec;
		strftime(time_buff, 100, "%F %H:%M", localtime(&time));
		if((&log[i])->ut_type == 1 && runlevel == 0){
			printf("         %s %c   %s\n", (&log[i])->ut_user, (&log[i])->ut_pid % 256, time_buff);
			runlevel++;
		}
	} 
}

// Normal user processes 7
void who_u(void){
	for(int i = 0; i < logsize; i++){
		if((&log[i])->ut_type == 7){
			time = (&log[i])->ut_tv.tv_sec;
			strftime(time_buff, 100, "%F %H:%M", localtime(&time));
			printf("%s   %s         %s         %d\n", (&log[i])->ut_user, (&log[i])->ut_line, time_buff, (&log[i])->ut_pid);
		}
	} 
}

/* Function that prints the help */
void help(void){
	printf(BOLD "USAGE\n" RESET);
    printf("    mywho [option]\n\n");
    printf(BOLD "DESCRIPTION\n" RESET);
    printf("    Print information about users who are currently logged in.\n\n");
    printf(BOLD "    -a, --all " RESET);
    printf("print all processes\n\n");
    printf(BOLD "    -b, --boot " RESET);
    printf("time of last system boot\n\n");
    printf(BOLD "    -d, --dead " RESET);
    printf("print dead processes\n\n");
    printf(BOLD "    -h, --help " RESET);
    printf("display this help and exit\n\n");
    printf(BOLD "    -l, --login " RESET);
    printf("print system login processes\n\n");
    printf(BOLD "    -q, --count " RESET);
    printf("all login names and number of users logged on\n\n");
    printf(BOLD "    -r, --runlevel " RESET);
    printf("print current runlevel\n\n");
    printf(BOLD "    -u, --users " RESET);
    printf("list users logged in\n\n");
    printf(BOLD "AUTHORS\n" RESET);
    printf("    Written by ");
    printf(BOLD YELLOW"Othman MEJDOUBI " RESET);
    printf("& ");
    printf(BOLD YELLOW"Zakaria CHIHI" RESET);
    printf("\n\n");
}

while ((c = getopt_long (argc, argv, "abdhlqru", longopts, NULL)) != -1) {
	switch (c) {
	case 'a':
		do_all = 1;
		break ;
	case 'b':
		do_boot = 1;
		break ;
	case 'd':
		do_dead = 1;
		break ;
	case 'h':
		do_help = 1;
		break ;
	case 'l':
		do_login = 1;
		break ;
	case 'q':
		do_count = 1;
		break ;
	case 'r':
		do_runlevel = 1;
		break ;
	case 'u':
		do_users = 1;
		break ;
	case 0:
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
	// who -a
	if(do_all == 1 && do_count == 0){
    	who_a();
	}
	// who -b
	if(do_all == 0 && do_boot == 1 && do_count == 0){
    	who_b();
	}
	// who -d
	if(do_all == 0 && do_dead == 1 && do_count == 0){
    	who_d();
	}
	// who -h
	if(do_all == 0 && do_boot == 0 && do_dead == 0 && do_help == 1 && do_login == 0 
		&& do_count == 0 && do_runlevel == 0 && do_users == 0){
    	help();
	}
	// who -l
	if(do_all == 0 && do_login == 1 && do_count == 0){
    	who_l();
	}
	// who -q
	if(do_count == 1){
   		who_q();
	}
	// who -r
	if(do_all == 0 && do_runlevel == 1 && do_count == 0){
    	who_r();
	}
	// who -u
	if(do_all == 0 && do_count == 0 && do_users == 1){
    	who_u();
	}
	// who
	if(do_all == 0 && do_boot == 0 && do_dead == 0 && do_help == 0 && do_login == 0 
		&& do_count == 0 && do_runlevel == 0 && do_users == 0 && invalid_option == 0){
		who();
	}
}

}