/*
 *  SMALL SHELL WRITTEN BY JOSH NEGREANU (OPERATING SYSTEMS ASSIGNMENT 3 MAY 22, 2023)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>
#include <dirent.h>

#define _XOPEN_SOURCE 700

// universal declarations
#define INPUT_SIZE 2048
#define COMMAND_SIZE 512
#define PROC_SIZE 256

// boolean for entering/exiting foreground-only mode
volatile int ampersand_mode = 1;

// terminal stop handler
void ampersand_handler(int dummy) {
    // entering foreground-only mode
    if (ampersand_mode) {
        char* message = "\n[!] entering foreground-only mode (& is now ignored)\n";
	    write(STDOUT_FILENO, message, 55);
        fflush(stdout);
        ampersand_mode = 0;
    
    // exiting foreground-only mode
    } else {
        char* message = "\n[!] exiting foreground-only mode\n";
	    write(STDOUT_FILENO, message, 35);
        fflush(stdout);
        ampersand_mode = 1;
    }
}

int main() {
    // system vars
    int status = 0;
    int exited = 1;
    char current_cwd[256];
    char future_cwd[256];

    // background child process array
    int child_procs[PROC_SIZE];
    int num_procs = 0;

    // initialize user input
    char user_input[INPUT_SIZE];

    // initialize argument array
    char *argv[COMMAND_SIZE];

    // get currently path
    getcwd(current_cwd, sizeof(current_cwd));

    // creation of sigactions
    struct sigaction sigint = {{0}}, sigtstp = {{0}};

    // sigint
    sigint.sa_handler = SIG_IGN;
    sigfillset(&sigint.sa_mask);
    sigint.sa_flags = SA_RESTART;

    // sigtstp
    sigtstp.sa_handler = ampersand_handler;
    sigfillset(&sigtstp.sa_mask);
    sigtstp.sa_flags = 0;

    // sigaction activation
    sigaction(SIGTSTP, &sigtstp, NULL);
    sigaction(SIGINT, &sigint, NULL);

    while (1) {
        // get the ecurrent directory
        getcwd(current_cwd, sizeof(current_cwd));

        // prompt user
        printf(": ");
        fflush(stdout);

        // reset terminal stop flags
        sigtstp.sa_flags = 0;
        sigaction(SIGTSTP, &sigtstp, NULL);

        // get user input
        if (fgets(user_input, INPUT_SIZE, stdin) == NULL)
            continue;

         // non-empty or comment command
        if (strcmp(user_input, "\n") != 0 && strncmp(user_input, "#", 1) != 0) {
            // removing enter key
            user_input[strlen(user_input)-1] = '\0';

            // reset argument counter
            int argc = 0;

            // creating tokenizer
            char* str_ptr = strtok(user_input, " ");

            // parsing
            while (str_ptr != NULL) {
                char* temp = malloc(strlen (str_ptr) + 1);
                if (temp != NULL) {
                    strcpy(temp, str_ptr);
                }
                argv[argc++] = temp;
                str_ptr = strtok (NULL, " ");
            }
            argv[argc] = NULL;

            // expand $$ to pid
            int i;
            for (i = 0; i < argc; i ++) {
                char* temp;
                while ((temp = strstr(argv[i], "$$")) != NULL) {
                    // modify string to include pid instead of $$
                    char* new = malloc(sizeof(argv[i]) + 5);
                    sprintf(new, "%.*s%d%s", (int)(temp - argv[i]), argv[i], getpid(), temp+2);
                    argv[i] = new;
                }
            }

            // has ampersand
            int has_ampersand = !strcmp(argv[argc-1], "&");
            if (has_ampersand) {
                argv[argc-1] = NULL;
                argc -= 1;
            }

            // exit command
            if (strcmp(argv[0], "exit") == 0 && argc == 1) {
                // run through all the processes in the background
                int i;
                for (i = 0; i < PROC_SIZE; i ++) {
                    if (child_procs[i] != 0) {
                        // kill the process if it exists
                        kill(child_procs[i], 9);
                    }
                }
                
                // exit the program with error code 0
                exit(0);

            // status command
            } else if (strcmp(argv[0], "status") == 0 && argc == 1) {
                // if foreground processes exited
                if (exited) {
                    printf("[!] exit value %d\n", status);
                
                // if foreground process terminated
                } else {
                    printf("[!] terminated by signal %d\n", status);
                }
                fflush(stdout);

            // cd command
            } else if (strcmp(argv[0], "cd") == 0) {
                if (argc == 1) {
                    // no change to current dir
                    chdir(getenv("HOME"));
                } else if (argc == 2) {
                    if (strncmp("/", argv[1], 1) == 0) {
                        // absolute directory
                        if (chdir(argv[1]) == 0) {
                            strcpy(current_cwd, argv[1]);
                        }
                    } else {
                        // relative directory
                        sprintf(future_cwd, "%s/%s", current_cwd, argv[1]);

                        // if dir change is successful, update current dir
                        if (chdir(future_cwd) == 0) {
                            strcpy(current_cwd, future_cwd);
                        }
                    }
                }

            // more complex command
            } else {
                // forking
                pid_t pid = fork();

                // allow restart for terminal stop
                sigtstp.sa_flags = SA_RESTART;
                sigaction(SIGTSTP, &sigtstp, NULL);

                // fork error handling
                if (pid < 0) {
                    perror("[!] fork() failure");
                    exit(1);

                // child process
                } else if (pid == 0) {
                    // redirection variables
                    int redirection_success = 1;

                    // ignore terminal stop
                    sigtstp.sa_handler = SIG_IGN;
                    sigaction(SIGTSTP, &sigtstp, NULL);

                    // declare standard in/out for foreground and background processes
                    int in = 0;
                    int background_in = open("/dev/null", O_RDWR, 0777);
                    int out = 1;
                    int background_out = open("/dev/null", O_RDWR, 0777);

                    // redirection
                    int i;
                    // cycle through the arguments
                    for (i = 0; i < argc; i ++) {
                        if (strcmp(argv[i], "<") == 0 && i < argc-1) {
                            // open the input redirection file
                            background_in = in = open(argv[i+1], O_RDONLY);

                            // redirection error
                            if (in == -1) {
                                redirection_success = 0;
                                break;
                            }

                            // remove arguments and move everything down
                            int j;
                            for (j = i+2; j < argc; j ++) {
                                argv[j-2] = argv[j];
                            }

                            // remove last two spots (empty arguments)
                            argv[argc-1] = NULL;
                            argv[argc-2] = NULL;
                            argc -= 2;
                            i --;

                        } else if (strcmp(argv[i], ">") == 0 && i < argc-1) {
                            // open the output redirection file
                            background_out = out = open(argv[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0777);

                            // redirection error
                            if (out == -1)  {
                                redirection_success = 0;
                                break;
                            }

                            // remove arguments and move everything down
                            int j;
                            for (j = i+2; j < argc; j ++) {
                                argv[j-2] = argv[j];
                            }

                            // remove last two spots (empty arguments)
                            argv[argc-1] = NULL;
                            argv[argc-2] = NULL;
                            argc -= 2;
                            i --;
                        }
                    }

                    // background process
                    if (has_ampersand && ampersand_mode) {
                        // if redirection was a success
                        if (redirection_success) {
                            // if you haven't exceeded the number of background procs available
                            if (num_procs != PROC_SIZE) {
                                // change the input and output
                                dup2(background_in, 0);
                                dup2(background_out, 1);
                                
                                // execute the command
                                execvp(argv[0], argv);
                                // command error
                                printf("[!] command \"%s\" does not exist or used incorrectly\n", argv[0]);
                                fflush(stdout);
                                
                                // exit with an error
                                exit(1);
                            } else {
                                exit(1);
                            }
                        } else {
                            // redirection failure
                            printf("[!] redirection failure\n");
                            fflush(stdout);
                            exit(1);
                        }

                    // foreground process
                    } else {
                        if (redirection_success) {
                            // modification to sigint
                            sigint.sa_handler = SIG_DFL;
                            sigaction(SIGINT, &sigint, NULL);

                            // change the input and output
                            dup2(out, 1);
                            dup2(in, 0);
                            
                            // execute the command
                            execvp(argv[0], argv);
                            // command error
                            printf("[!] command \"%s\" does not exist or used incorrectly\n", argv[0]);
                            fflush(stdout);
                            
                            // exit with an error
                            exit(1);
                        } else {
                            // redirection failure
                            printf("[!] redirection failure\n");
                            fflush(stdout);
                            exit(1);
                        }
                    }

                // parent process
                } else {
                    // check if command is running in the background
                    if (has_ampersand && ampersand_mode) {
                        //check if the number of background processes isn't exceeded yet
                        if (num_procs != PROC_SIZE) {
                            // run through all current processes
                            for (i = 0; i < PROC_SIZE; i ++) {
                                // if there's an empty space
                                if (child_procs[i] == 0) {
                                    // fill up that space with the newly created process
                                    child_procs[i] = pid;
                                    num_procs ++;
                                    printf("[%d] background pid is %d\n", num_procs, pid);
                                    fflush(stdout);
                                    break;
                                }
                            }

                        // otherwise, print an error saying number of background processes is exceeded
                        } else {
                            printf("[%d] only %d background processes can run at a given time\n", num_procs, PROC_SIZE);
                            fflush(stdout);
                        }

                    } else {
                        // hang parent process until child process is done
                        int temp_status = 0;
                        
                        // wait for child to terminate
                        if (waitpid(pid, &temp_status, 0) != pid) {
                            printf("[!] waitpid() failure");
                        }

                        // determine mode of exit
                        if(WIFEXITED(temp_status)){
                            status = WEXITSTATUS(temp_status);
                            exited = 1;
                        } else{
                            status = WTERMSIG(temp_status);
                            printf("\n[!] terminated by signal %d\n", status);
                            fflush(stdout);
                            exited = 0;
                        }
                    }
                }
            }
        }

        // run through all the current processes
        int i;
        for (i = 0; i < PROC_SIZE; i ++) {
            
            if (child_procs[i] != 0) {
                int back_status = 0;
                
                // check if process is done
                if (waitpid(child_procs[i], &back_status, WNOHANG) == child_procs[i]) {
                    // print to console and remove process from list
                    // determine mode of exit
                    if(WIFEXITED(back_status)){
                        // if background process exited, print exit value
                        printf("[%d] background pid %d is done: exit value %d\n", num_procs, child_procs[i], WEXITSTATUS(back_status));
                    } else {
                        // if background terminated, print terminating signal
                        printf("[%d] background pid %d is done: terminated by signal %d\n", num_procs, child_procs[i], WTERMSIG(back_status));
                    }
                    fflush(stdout);
                    child_procs[i] = 0;
                    num_procs --;
                }
            }
        }
    }
}