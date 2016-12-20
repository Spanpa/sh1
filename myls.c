/*  ECAM 2016
    Othman MEJDOUBI & Zakariya CHIHI
    myls : list directory contents.
    usage : myls [option]... [file]...
    4 options :
        -a, --all        do not ignore entries starting with .
        -h, --help       display the help
        -l,              use a long listing format
        -R, --recurisve  list subdirectories recursively */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <sys/syscall.h>

#define BLUE        "\x1b[34m"
#define YELLOW      "\x1b[33m"
#define BOLD 	    "\x1b[1m"
#define RESET       "\x1b[0m"

int main(int argc, char **argv){

struct option longopts [] = {
    { "all", no_argument , NULL , 'a' },
    { "help", no_argument , NULL , 'h' },
    { "recursive", no_argument , NULL , 'R' },
    { 0, no_argument , NULL , 'l' },
    { 0, 0, 0, 0 }
};

struct linux_dirent {
    long           d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    char           d_name[];
};

int c;
char *ls_value = NULL;
int do_all = 0;
int do_help = 0;
int do_l = 0;
int do_recursive = 0;
int invalid_option = 0;

int fd, n;
char buf[1024];
struct linux_dirent *namelist;
struct stat fileStat;

char **directories = malloc(1024 * sizeof(char*));
char file_name[255];
char *file_path;
char full_path[255];
char time_buff[100];
char d_type;

void file_stat(void){
    // stats the file pointed to by full_path and fills in fileStat
    stat(full_path, &fileStat);
    // file permissions
    printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
    printf(" ");
    // user ID and group ID of file owner
    printf("%ld %ld ", (long)fileStat.st_uid, (long)fileStat.st_gid);
    // total file size in bytes
    printf("%lld ",(long long)fileStat.st_size);
    // last file modification
    strftime(time_buff, 100, "%b %d %H:%M", localtime(&fileStat.st_mtime));
    printf("%s ",time_buff);
}

void ls(char *path){
    file_path = path;
    if(file_path == NULL){
        file_path = ".";
    }
    // open file_path file or directory and get the file descriptor
    fd = open(file_path, O_RDONLY | O_DIRECTORY);

    do{
        /* reads several linux_dirent structures from the directory referred to 
           by the open file descriptor fd into the buffer pointed to by buf. */
        n = syscall(SYS_getdents, fd, buf, 1024);
        if(n == -1){
            printf("Error : Unknown directory.");
        }
        for(int i = 0; i < n;) {
            namelist = (struct linux_dirent *) (buf + i);
            // store file type in d_type
            d_type = *(buf + i + namelist->d_reclen - 1);
            strncpy(file_name, namelist->d_name, 254);
            // ignore entries starting by "."
            if(file_name[0] !=  '.'){
                // print directory entries in blue
            	if(d_type == 4){
            		printf(BLUE BOLD    "%s "     RESET, namelist->d_name);
            	}
                // print file entries
            	else {
            		printf("%s ", namelist->d_name);
            	}
            }
            i += namelist->d_reclen;
        }
	}while(n > 0);
    printf("\n");
}

void ls_a(void){
    if(ls_value == NULL){
        file_path = ".";
    }
    else{
        file_path = ls_value;
    }

    fd = open(file_path, O_RDONLY | O_DIRECTORY);

    do{
        n = syscall(SYS_getdents, fd, buf, 1024);
        if(n == -1){
            printf("Error : Unknown directory.");
        }
        for (int i = 0; i < n;) {
            namelist = (struct linux_dirent *) (buf + i);
            d_type = *(buf + i + namelist->d_reclen - 1);
            strncpy(file_name, namelist->d_name, 254);
            // does not ignore entries starting by "."
            if(d_type == 4){
                printf(BLUE BOLD    "%s "     RESET, namelist->d_name);
            }
            else {
                printf("%s ", namelist->d_name);
            }
            i += namelist->d_reclen;
        }
    }while(n > 0);
    printf("\n");
}

void ls_l(void){
    if(ls_value == NULL){
        file_path = ".";
    }
    else{
        file_path = ls_value;
    }

    fd = open(file_path, O_RDONLY | O_DIRECTORY);

    do{
        n = syscall(SYS_getdents, fd, buf, 1024);
        if(n == -1){
            printf("Error : Unknown directory.");
        }
        for (int i = 0; i < n;) {
            strncpy(full_path, file_path, 254);
            namelist = (struct linux_dirent *) (buf + i);
            d_type = *(buf + i + namelist->d_reclen - 1);
            strncpy(file_name, namelist->d_name, 254);
            if(file_name[0] !=  '.'){
                // verifies the full path
                if(full_path[(strlen(full_path)-1)] == '/'){
                    strcat(full_path, namelist->d_name);
                }
                else {
                    strcat(full_path, "/");
                    strcat(full_path, namelist->d_name);
                }
                // prints file stats
                file_stat();
                if(d_type == 4){
                    printf(BLUE BOLD    "%s "     RESET, namelist->d_name);
                }
                else {
                    printf("%s ", namelist->d_name);
                }
                printf("\n");
            }
            i += namelist->d_reclen;
        }
    }while(n > 0);
}

void ls_R(char *path){
    int j = 0;
    file_path = path;
    if(file_path == NULL){
        file_path = ".";
    }

    fd = open(file_path, O_RDONLY | O_DIRECTORY);

    do{
        n = syscall(SYS_getdents, fd, buf, 1024);
        if(n == -1){
            printf("Error : Unknown directory.");
        }
        if(n > 0){  
            printf("%s:\n", file_path);
        }
        for (int i = 0; i < n;) {
            strncpy(full_path, file_path, 254);
            namelist = (struct linux_dirent *) (buf + i);
            d_type = *(buf + i + namelist->d_reclen - 1);
            strncpy(file_name, namelist->d_name, 254);
            if(file_name[0] !=  '.'){
                if(d_type == 4){
                    printf(BLUE BOLD    "%s "     RESET, namelist->d_name);
                    if(full_path[(strlen(full_path)-1)] == '/'){
                        strcat(full_path, namelist->d_name);
                    }
                    else {
                        strcat(full_path, "/");
                        strcat(full_path, namelist->d_name);
                    }
                    // every subdirectory found in file_path is stored in directories
                    directories[j] = malloc(64 * sizeof(char));
                    strcpy(directories[j], full_path);
                    j++;
                }
                else {
                    printf("%s ", namelist->d_name);
                }
            }
            i += namelist->d_reclen;
        }
    }while(n > 0);
    printf("\n\n");
    // get entries of all subdirectories
    for(int k = 0; k < j; k++){
        ls_R(directories[k]);
    }
}


void ls_aR(char *path){
    int j = 0;
    file_path = path;
    if(file_path == NULL){
        file_path = ".";
    }

    fd = open(file_path, O_RDONLY | O_DIRECTORY);

    do{
        n = syscall(SYS_getdents, fd, buf, 1024);
        if(n == -1){
            printf("Error : Unknown directory.");
        }
        if(n > 0){  
            printf("%s:\n", file_path);
        }
        for (int i = 0; i < n;) {
            strncpy(full_path, file_path, 254);
            namelist = (struct linux_dirent *) (buf + i);
            d_type = *(buf + i + namelist->d_reclen - 1);
            strncpy(file_name, namelist->d_name, 254);
            if(d_type == 4){
                printf(BLUE BOLD    "%s "     RESET, namelist->d_name);
                if(full_path[(strlen(full_path)-1)] == '/'){
                    strcat(full_path, namelist->d_name);
                }
                else {
                    strcat(full_path, "/");
                    strcat(full_path, namelist->d_name);
                }
                if(file_name[0] !=  '.'){
                    directories[j] = malloc(64 * sizeof(char));
                    strcpy(directories[j], full_path);
                    j++;
                }
            }
            else {
                printf("%s ", namelist->d_name);
            }
            i += namelist->d_reclen;
        }
    }while(n > 0);
    printf("\n\n");
    for(int k = 0; k < j; k++){
        ls_aR(directories[k]);
    }
}

void ls_lR(char *path){
    int j = 0;
    file_path = path;
    if(file_path == NULL){
        file_path = ".";
    }

    fd = open(file_path, O_RDONLY | O_DIRECTORY);

    do{
        n = syscall(SYS_getdents, fd, buf, 1024);
        if(n == -1){
            printf("Error : Unknown directory.");
        }
        if(n > 0){  
            printf("%s:\n", file_path);
        }
        for (int i = 0; i < n;) {
            strncpy(full_path, file_path, 254);
            namelist = (struct linux_dirent *) (buf + i);
            d_type = *(buf + i + namelist->d_reclen - 1);
            strncpy(file_name, namelist->d_name, 254);
            if(file_name[0] !=  '.'){
                if(full_path[(strlen(full_path)-1)] == '/'){
                    strcat(full_path, namelist->d_name);
                }
                else {
                    strcat(full_path, "/");
                    strcat(full_path, namelist->d_name);
                }

                if(d_type == 4){
                    printf(BLUE BOLD    "%s "     RESET, namelist->d_name);
                    directories[j] = malloc(64 * sizeof(char));
                    strcpy(directories[j], full_path);
                    j++;
                }
                else {
                    printf("%s ", namelist->d_name);
                }
                printf("\n");
            }
            i += namelist->d_reclen;
        }
    }while(n > 0);
    printf("\n");
    for(int k = 0; k < j; k++){
        ls_lR(directories[k]);
    }
}

void ls_alR(char *path){
    int j = 0;
    file_path = path;
    if(file_path == NULL){
        file_path = ".";
    }

    fd = open(file_path, O_RDONLY | O_DIRECTORY);

    do{
        n = syscall(SYS_getdents, fd, buf, 1024);
        if(n == -1){
            printf("Error : Unknown directory.");
        }
        if(n > 0){  
            printf("%s:\n", file_path);
        }
        for (int i = 0; i < n;) {
            strncpy(full_path, file_path, 254);
            namelist = (struct linux_dirent *) (buf + i);
            d_type = *(buf + i + namelist->d_reclen - 1);
            strncpy(file_name, namelist->d_name, 254);
            if(full_path[(strlen(full_path)-1)] == '/'){
                strcat(full_path, namelist->d_name);
            }
            else {
                strcat(full_path, "/");
                strcat(full_path, namelist->d_name);
            }
            file_stat();
            if(d_type == 4){
                printf(BLUE BOLD    "%s "     RESET, namelist->d_name);
                if(file_name[0] !=  '.'){
                    directories[j] = malloc(64 * sizeof(char));
                    strcpy(directories[j], full_path);
                    j++;
                }
            }
            else {
                printf("%s ", namelist->d_name);
            }
            printf("\n");
            i += namelist->d_reclen;
        }
    }while(n > 0);
    printf("\n");
    for(int k = 0; k < j; k++){
        ls_alR(directories[k]);
    }
}

void ls_al(void){
    if(ls_value == NULL){
        file_path = ".";
    }
    else{
        file_path = ls_value;
    }

    fd = open(file_path, O_RDONLY | O_DIRECTORY);

    do{
        n = syscall(SYS_getdents, fd, buf, 1024);
        if(n == -1){
            printf("Error : Unknown directory.");
        }
        for (int i = 0; i < n;) {
            strncpy(full_path, file_path, 254);
            namelist = (struct linux_dirent *) (buf + i);
            d_type = *(buf + i + namelist->d_reclen - 1);
            strncpy(file_name, namelist->d_name, 254);
            if(full_path[(strlen(full_path)-1)] == '/'){
                strcat(full_path, namelist->d_name);
            }
            else {
                strcat(full_path, "/");
                strcat(full_path, namelist->d_name);
            }
            file_stat();
            if(d_type == 4){
                printf(BLUE BOLD    "%s "     RESET, namelist->d_name);
            }
            else {
                printf("%s ", namelist->d_name);
            }
            printf("\n");
            i += namelist->d_reclen;
        }
    }while(n > 0);
}

/* Function that prints the help */
void help(void){
    printf(BOLD "USAGE\n" RESET);
    printf("    myls [option]... [file]...\n\n");
    printf(BOLD "DESCRIPTION\n" RESET);
    printf("    List directory contents.\n\n");
    printf(BOLD "    -a, --all " RESET);
    printf("do not ignore entries starting with .\n\n");
    printf(BOLD "    -h, --help " RESET);
    printf("display this help and exit\n\n");
    printf(BOLD "    -l,  " RESET);
    printf("use a long listing format\n\n");
    printf(BOLD "    -R, --recursive " RESET);
    printf("list subdirectories recursively\n\n");
    printf("    Written by ");
    printf(BOLD YELLOW"Othman MEJDOUBI " RESET);
    printf("& ");
    printf(BOLD YELLOW"Zakaria CHIHI" RESET);
    printf("\n\n");
}

while ((c = getopt_long (argc, argv, "ahlR", longopts, NULL)) != -1) {
	switch (c) {
	case 'a':
		do_all = 1;
		ls_value = argv[2];
		break ;
    case 'h':
        do_help = 1;
        break ;
	case 'l':
		do_l = 1;
		ls_value = argv[2];
		break ;
	case 'R':
		do_recursive = 1;
		ls_value = argv[2];
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
    else if(do_all == 1 && do_l == 0 && do_recursive == 0){
        ls_a();
    }
    else if(do_all == 0 && do_l == 1 && do_recursive == 0){
        ls_l();
    }
    else if(do_all == 0 && do_l == 0 && do_recursive == 1){
        ls_R(ls_value);
    }
    else if(do_all == 1 && do_l == 1 && do_recursive == 0){
        ls_al();
    }
    else if(do_all == 1 && do_l == 0 && do_recursive == 1){
        ls_aR(ls_value);
    }
    else if(do_all == 0 && do_l == 1 && do_recursive == 1){
        ls_lR(ls_value);
    }
    else if(do_all == 1 && do_l == 1 && do_recursive == 1){
        ls_alR(ls_value);
    }
    else if(do_all == 0 && do_l == 0 && do_recursive == 0){
        ls_value = argv[1];
        ls(ls_value);
    }
}

}