/*  ECAM 2016
    Othman MEJDOUBI & Zakaria CHIHI
    sh1 is a simple implementation of a shell in C. 
    It includes the following spec/limitations :
        Interactive and batch modes available.
        Commands must be on a single line.
        Arguments must be separated by whitespace.
        Only builtins are: myls, myps, myrm, mywho, mypwd, help and exit. */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define YELLOW      "\x1b[33m"
#define GREEN       "\x1b[32m"
#define BOLD        "\x1b[1m"
#define RESET       "\x1b[0m"

/* List of available commands */
char *available_commands[7] = {
  "myls",
  "myps",
  "myrm",
  "mywho",
  "mypwd",
  "help",
  "exit"
};

/* Struct with the number of tokens a buffer/line is splitted in
   and the tokens */
struct Rsplit {
    int number;
    char **tokens;
};

/* Function that returns 1 when the command is available */
int check_command(char **args){
    for(int i = 0; i < 7; i++){
        if(strcmp(args[0], available_commands[i]) == 0){
            return 1;
            break;
        }
    }
    return 0;   
}

/* Function that prints the help */
void help(void)
{
  printf("Shell 1 : ");
  printf(BOLD YELLOW"Othman MEJDOUBI " RESET);
  printf("& ");
  printf(BOLD YELLOW"Zakaria CHIHI\n" RESET);
  printf("Type command and arguments, and voidhit enter.\n");
  printf("The following are built in:\n");

  for (int i = 0; i < 7; i++) {
    printf(" -%s\n", available_commands[i]);
  }

  printf("For specific command help, type command -h or --help (e.g., " GREEN BOLD "myls -h" RESET ").\n");
}

/* Function that returns the line retrieved from stdin with getline() */
char *read_line(){
    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, stdin);
    // remove newline character
    line = strtok(line, "\n");
    return line;
}

/* Function that breaks a buffer/line into a series of tokens
   using the delimiter delim */
struct Rsplit split(char *line, char *delim){
    char *token;
    char **tokens = malloc(64 * sizeof(char*));
    int i = 0;
    // get the first token
    token  = strtok(line, delim);
    tokens[0] = token;
    // walk through other tokens and store them
    while(token != NULL){
        token  = strtok(NULL, delim);
        tokens[++i] = token;
    }
    struct Rsplit r = {i, tokens};
    return r;
}

/* Function that starts a process by duplicating an existing process
   with fork(). The original process is called the "parent",
   and the new one is called the "child". The execv() system call 
   replaces the current running program with a new one */
int execute(char **args){
    pid_t pid = fork();
    if(pid < 0){
        // error forking
        perror("lsh");
    }
    else if(pid == 0){
        // child process
        execv(args[0], args);
    }
    else{
        // parent process
        wait(NULL);
        // child has finished
    }
    return 1;
}

/* Function that handles a command with 4 steps :
   - read the command from standard input
   - separate the command string into a program and arguments
   - check if the command is built-in
   - run the parsed command */
void shell_loop(void){
    char *line;
    char **args;
    struct Rsplit r;
    int status;
    do{
        // print the prompt
        printf("sh1 > ");
        // read the command
        line = read_line();
        if(line != NULL){
            r = split(line, " ");
            // retrieve the command arguments
            args = r.tokens;
            // check if the command is built-in
            if(check_command(args) == 1){
                if(strcmp(args[0], "help") == 0){
                    status = 1;
                    help();
                }
                else if(strcmp(args[0], "exit") == 0){
                    status = 0;
                    printf("GOOD BYE\n");
                }
                else{
                    // execute the command given its arguments
                    status = execute(args);
                }
            }
            else{
                status = 1;
                printf("Unknown command, try help to get the available commands.\n");
            }
        }
        else{
            status = 1;
        }
        // perform a cleanup
        free(line);
        free(args);
    }while(status);  
}

/* Function that handles a list of commands given with 4 steps :
   - read the commands list from a batch file
   - separate the commands
   and for each command :
   - separate the command string into a program and arguments
   - check if the command is built-in
   - run the parsed command */
void batch(char *path){
    char buffer[1024];
    char **commands;
    char *line;
    char **args;
    struct Rsplit r;
    struct Rsplit s;
    // open the file where the commands list is stored
    int fd = open(path, O_RDONLY);
    if(fd == -1){
        perror("open");
    }
    else{
        // read the file and store its contents into a buffer
        read(fd, buffer, sizeof(buffer));
        // split each line of the file(buffer) into commands
        r = split(buffer, "\n");
        commands = r.tokens;
        for(int i = 0; i < (r.number - 1); i++){
            line = commands[i];
            printf("%s\n", line);
            if(line != NULL){
                // split the command into arguments
                s = split(line, " ");
                args = s.tokens;
                // check if the commmand is built-in
                if(check_command(args) == 1){
                    if(strcmp(args[0], "help") == 0){
                        help();
                    }
                    else if(strcmp(args[0], "exit") == 0){
                        printf("GOOD BYE \n");
                        break;
                    }
                    else{
                        // execute the command given its arguments
                        execute(args);
                    }
                }
                else{
                    printf("Unknown command %s, try help to get the available commands.\n", args[0]);
                }
            }
        }
        close(fd);
    }
}

int main(int argc, char **argv){
    if(argv[1] != NULL){
        // batch mode
        batch(argv[1]);
    }
    else{
        // interactive mode
        shell_loop();
    }
    return EXIT_SUCCESS;
}