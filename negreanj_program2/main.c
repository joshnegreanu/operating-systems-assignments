#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "movie.h"
#include "language.h"

#define PREFIX "movies_"
#define EXTENSION ".csv"

void run_file(char* file_name) {
    
    // parse file into movie list
    struct movie* movie_list = parse_file(file_name);
    
    // print file name and list number of movies
    printf("\nNow processing the chosen file named %s\n", file_name);

    // create a directory name
    char output_file_name[30] =  "negreanj.movies.";

    // generate a random number between 0 and 99999
    int random_num = rand() % 99999;

    // merge the directory name with the number into the directory name
    sprintf(output_file_name, "%s%d", output_file_name, random_num);

    // create a directory with that name with rwx for user and r-x for the group
    mkdir(output_file_name, S_IRWXU | S_IRGRP | S_IXGRP);
    printf("\nCreated directory with name %s\n", output_file_name);

    // loop through the movie linked list
    struct movie* movie_ptr = movie_list;
    while (movie_ptr != NULL) {
        // create a complete file directory for the new text file to be created
        char file_dir[40];

        // merge the directory name with the current year with the .txt extension
        sprintf(file_dir, "./%s/%d.txt", output_file_name, get_year(movie_ptr));

        // open the file to append (which creates if it doesn't extist)
        FILE* file_ptr = NULL;
        
        file_ptr = fopen(file_dir ,"a");

        // modify mode for rw- for the user and r-- for the group
        chmod(file_dir, S_IRGRP | S_IRUSR | S_IWUSR);

        // add the movie name to the end of the file
        fprintf(file_ptr, "%s\n", get_title(movie_ptr));

        //close the file
        fclose(file_ptr);

        movie_ptr = get_next_movie(movie_ptr);
    }
}

int main(int argc, char *argv[]) {
    // generate a random generator using clock
    time_t t;
    srand((unsigned) time(&t));

    int user_option_num = 0;

    while (1) {
        // show user input options
        printf("\n1. Select file to process\n");
        printf("2. Exit the program\n");
        printf("\nEnter a choice from 1 to 2: ");

        // get user input number
        scanf("%d", &user_option_num);

        if (user_option_num == 1) {
            int file_option_num = 0;

            // open current directory
            // code used from Exploration: Directories
            DIR* currDir = opendir(".");
            struct dirent *aDir;
            struct stat dirStat;
            int i = 0;
            char entryName[256];
            
            while (1) {
                // show user input options
                printf("\nWhich file you want to process?\n");
                printf("Enter 1 to pick the largest file\n");
                printf("Enter 2 to pick the smallest file\n");
                printf("Enter 3 to specify the name of a file\n");
                printf("Enter a choice from 1 to 3: ");

                // get user input
                scanf("%d", &file_option_num);

                if (file_option_num == 1) {
                    // user wants the program to find the largest file
                    // file starts with movies_ and has the .csv extension
                    off_t largest_size;

                    while ((aDir = readdir(currDir)) != NULL) {
                        // compare prefix
                        if(strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0){
                            // compare extension
                            if (strcmp(EXTENSION, aDir->d_name+(strlen(aDir->d_name)-4)) == 0) {
                                stat(aDir->d_name, &dirStat);

                                // keep track of size
                                // modify current largest file name
                                if(i == 0 || dirStat.st_size > largest_size) {
                                    largest_size = dirStat.st_size;
                                    memset(entryName, '\0', sizeof(entryName));
                                    strcpy(entryName, aDir->d_name);
                                }
                                i++;
                            }
                        }
                    }

                    break;

                } else if (file_option_num == 2) {
                    // user wants the program to find the smallest file
                    // file starts with movies_ and has the .csv extension
                    off_t smallest_size;

                    while ((aDir = readdir(currDir)) != NULL) {
                        // compare prefix
                        if(strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0){
                            // compare extension
                            if (strcmp(EXTENSION, aDir->d_name+(strlen(aDir->d_name)-4)) == 0) {
                                stat(aDir->d_name, &dirStat);  
                            
                                // keep track of size
                                // modify current largest file name
                                if(i == 0 || dirStat.st_size < smallest_size) {
                                    smallest_size = dirStat.st_size;
                                    memset(entryName, '\0', sizeof(entryName));
                                    strcpy(entryName, aDir->d_name);
                                }
                                i++;
                            }
                        }
                    }
                    break;

                } else if (file_option_num == 3) {
                    // user wants the program to find a custom file
                    // file starts with movies_ and has the .csv extension
                    char* custom_file_name = malloc(20 * sizeof(char));

                    // ask for the custom file name
                    printf("\nEnter the complete file name: ");
                    scanf("%s", custom_file_name);

                    int does_exist = 0;

                    while ((aDir = readdir(currDir)) != NULL) {
                        if (strcmp(custom_file_name, aDir->d_name) == 0) {
                            // program finds the file
                            does_exist = 1;
                        }
                    }

                    if (does_exist == 1) {
                        // file exists
                        strcpy(entryName, custom_file_name);
                        break;
                    } else {
                        // file does not exist
                        // close directory to repoen it again and reprompt the user
                        printf("\nThe file %s was not found. Try again\n", custom_file_name);
                        closedir(currDir);
                        currDir = opendir(".");
                    }

                } else {
                    // print error message and reprompt user
                    printf("\nYou entered an incorrect choice. Try again.\n");
                }
            }

            closedir(currDir);
            run_file(entryName);

        } else if (user_option_num == 2) {
            // break loop
            break;

        } else {
            // print error message and reprompt user
            printf("\nYou entered an incorrect choice. Try again.\n");

        }
    }

    return 0;
}