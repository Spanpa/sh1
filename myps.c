/*  ECAM 2016
	Othman MEJDOUBI & Zakariya CHIHI
	myps : report a snapshot of the current processes.
	usage : myps [options]
	3 options :
    	-a, --all 		print all processes except processes not associated with a terminal
    	-h, --help      display the help  
    	-u 		  		display with a long listing format */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <utmp.h>
#include <linux/kdev_t.h>

#define YELLOW      "\x1b[33m"
#define BOLD        "\x1b[1m"
#define RESET       "\x1b[0m"

int main(int argc, char **argv){

struct option longopts [] = {
	{ "all", no_argument , NULL , 'a' },
	{ "help", no_argument , NULL , 'h' },
	{ 0, no_argument , NULL , 'u' },
	{ 0, 0, 0, 0 }
};

struct linux_dirent {
    long           d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    char           d_name[];
};

/* Struct with the tty names and ids */
struct ttyids {
    char **ttys;
    char **ids;
};

int c;
int do_all = 0;
int do_u = 0;
int invalid_option = 0; 

int fd_proc, fd_cmd, fd, n;
int j = 0;
const char *proc = "/proc/";
const char *delim = " ";
char proc_stat[255];
char proc_cmdline[255];
struct linux_dirent *namelist;
struct stat fileStat;
char **stat_infos;
char buffer[1024];
char buffer_cmd[255];
char buf[1024];
char file_name[255];
char d_type;

/* Function that breaks a buffer into a series of tokens
   using the delimiter delim */
char **split_buffer(char *buffer){
	const char *delim = " ";
	char *token;
	char **tokens = malloc(1024 * sizeof(char*));
	int i = 0;
	// get the first token
	token  = strtok(buffer, delim);
	tokens[0] = token;
	// walk through other tokens and store them
	while(token != NULL){
		token  = strtok(NULL, delim);
		tokens[++i] = token;
	}
	return tokens;
}

struct ttyids who(void){
	char session_id[128];
	char **tty = malloc(64 * sizeof(char*));
	char **ids = malloc(64 * sizeof(char*));
	const int logsize = 16;
	int who_file;
	struct stat who_fileStat;
	// open utmp file and get the file descriptor who_file
	who_file = open("/var/run/utmp", O_RDONLY);
	struct utmp log[logsize];
	// read the file with who_file and store its contents into an array of struct utmp
	read(who_file, &log, logsize * sizeof (struct utmp));
	for(int i = 0; i < logsize; i++){
		// use only user processes
		if((&log[i])->ut_type == 7){
			snprintf(session_id, sizeof(session_id), "%d", (&log[i])->ut_session);
			// retrieve session ids and tty names of user processes
			ids[j] = malloc(5 * sizeof(char));
			tty[j] = malloc(16 * sizeof(char));
			// store into ids and ttys
			strcpy(ids[j], session_id);
			strcpy(tty[j], (&log[i])->ut_line);
			j++;
		}
	}
	struct ttyids t = {tty, ids};
	return t;
}

void ps(void){
	struct ttyids t = who();
	// open proc directory and get the file descriptor
	fd_proc = open(proc, O_RDONLY | O_DIRECTORY);
	do{
		/* reads several linux_dirent structures from the directory referred to 
           by the open file descriptor fd_proc into the buffer pointed to by buf. */
        n = syscall(SYS_getdents, fd_proc, buf, 1024);
        for (int i = 0; i < n;) {
            namelist = (struct linux_dirent *) (buf + i);
            // store file type in d_type
            d_type = *(buf + i + namelist->d_reclen - 1);
            strncpy(file_name, namelist->d_name, 254);
            // ignore entries starting by "."
            if(file_name[0] !=  '.'){
            	// use entries starting with a digit
            	if(file_name[0] >= '0' && file_name[0] <= '9'){
            		// use directories
            		if(d_type == 4){
            			//if(strcmp(file_name, "708") == 0){
            			strncpy(proc_stat, proc, 254);
            			// parsing the pid from the subdirectory name
            			strcat(proc_stat, file_name);
            			strcat(proc_stat, "/stat");
            			// open /proc/pid/stat file and get the file descriptor
            			fd = open(proc_stat, O_RDONLY);
            			// read the file and store its contents into a buffer
            			read(fd, buffer, sizeof(buffer));
            			// split the buffer into fields in stat_infos
            			stat_infos = split_buffer(buffer);
            			// walk through session ids retrived from who()
            			for(int k = 0; k < j; k++){
            				// print only the processes that have the same session id
            				if(strcmp(t.ids[k], stat_infos[5]) == 0){
            					// print process id, tty name, command name
            					printf("%s    %s    %s\n", stat_infos[0], t.ttys[k], stat_infos[1]);
            				}
            			}
						close(fd);
						//}
            		}
            	}
            }
            i += namelist->d_reclen;
        }
	}while(n > 0);
}

void ps_a(void){
	struct ttyids t = who();
	fd_proc = open(proc, O_RDONLY | O_DIRECTORY);
	do{
        n = syscall(SYS_getdents, fd_proc, buf, 1024);
        for (int i = 0; i < n;) {
            namelist = (struct linux_dirent *) (buf + i);
            d_type = *(buf + i + namelist->d_reclen - 1);
            strncpy(file_name, namelist->d_name, 254);
            if(file_name[0] !=  '.'){
            	if(file_name[0] >= '0' && file_name[0] <= '9'){
            		if(d_type == 4){
            			strncpy(proc_stat, proc, 254);
            			strcat(proc_stat, file_name);
            			strcat(proc_stat, "/stat");
            			fd = open(proc_stat, O_RDONLY);
            			read(fd, buffer, sizeof(buffer));
            			stat_infos = split_buffer(buffer);
            			for(int k = 0; k < j; k++){
            				// print only the processes that are user processes from all terminals
            				if(strcmp(t.ids[k], stat_infos[5]) == 0){
            					printf("%s    %s    %s\n", stat_infos[0], t.ttys[k], stat_infos[1]);
            				}
            			}
						close(fd);
            		}
            	}
            }
            i += namelist->d_reclen;
        }
	}while(n > 0);
}

void ps_u(void){
	struct ttyids t = who();
	fd_proc = open(proc, O_RDONLY | O_DIRECTORY);
	do{
        n = syscall(SYS_getdents, fd_proc, buf, 1024);
        for (int i = 0; i < n;) {
            namelist = (struct linux_dirent *) (buf + i);
            d_type = *(buf + i + namelist->d_reclen - 1);
            strncpy(file_name, namelist->d_name, 254);
            if(file_name[0] !=  '.'){
            	if(file_name[0] >= '0' && file_name[0] <= '9'){
            		if(d_type == 4){
            			strncpy(proc_stat, proc, 254);
            			strcat(proc_stat, file_name);
            			strncpy(proc_cmdline, proc_stat, 254);
            			strcat(proc_stat, "/stat");
            			strcat(proc_cmdline, "/cmdline");
            			fd = open(proc_stat, O_RDONLY);
            			fd_cmd = open(proc_cmdline, O_RDONLY);
            			read(fd, buffer, sizeof(buffer));
            			read(fd_cmd, buffer_cmd, sizeof(buffer_cmd));
            			stat_infos = split_buffer(buffer);
            			for(int k = 0; k < j; k++){
            				// print only the processes that are user processes from all terminals
            				if(strcmp(t.ids[k], stat_infos[5]) == 0){
            					// process id, resident set size, tty name, process state, cmdline(complete command line for the process)
            					printf("%s    %s    %s    %s    %s\n", stat_infos[0], stat_infos[23], t.ttys[k], stat_infos[2], buffer_cmd);
            				}
            			}
						close(fd);
            		}
            	}
            }
            i += namelist->d_reclen;
        }
	}while(n > 0);
}

/* Function that prints the help */
void help(void){
	printf(BOLD "USAGE\n" RESET);
    printf("    myps [options]\n\n");
    printf(BOLD "DESCRIPTION\n" RESET);
    printf("    Report a snapshot of the current processes.\n\n");
    printf(BOLD "    -a, --all " RESET);
    printf("print all processes\n\n");
    printf("    Written by ");
    printf(BOLD YELLOW"Othman MEJDOUBI " RESET);
    printf("& ");
    printf(BOLD YELLOW"Zakaria CHIHI" RESET);
    printf("\n\n");
}

while ((c = getopt_long (argc, argv, "au", longopts, NULL)) != -1) {
	switch (c) {
	case 'a':
		do_all = 1;
		break ;
	case 'u':
		do_u = 1;
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
	if(do_all == 1 && do_u == 0){
		ps_a();
	}
	else if(do_all == 0 && do_u == 1){
		ps_u();
	}
	else{
		ps();
	}
}

}