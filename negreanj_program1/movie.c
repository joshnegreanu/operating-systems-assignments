#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "movie.h"
#include "language.h"

// movie structure
struct movie {
    char* title;
    int year;
    struct language* language_list;
    float rating_value;

    struct movie* prev;
    struct movie* next;
};

// create_movie function
struct movie* create_movie(char* current_line) {
    struct movie* current_movie = malloc(sizeof(struct movie));

    char* token_pointer;

    //title
    char* token = strtok_r(current_line, ",", &token_pointer);
    current_movie->title = calloc(strlen(token) + 1, sizeof(char));
    strcpy(current_movie->title, token);

    //year
    token = strtok_r(NULL, ",", &token_pointer);
    current_movie->year = atoi(token);

    //languages
    token = strtok_r(NULL, ",", &token_pointer);
    current_movie->language_list = get_language_list(token);

    //rating_value
    token = strtok_r(NULL, ",", &token_pointer);
    current_movie->rating_value = strtod(token, NULL);

    current_movie->prev = NULL;
    current_movie->next = NULL;

    return current_movie;
}

// parse_file function
struct movie* parse_file(char *file_name) {
    FILE* movie_file = fopen(file_name, "r");

    char* current_line = NULL;
    size_t len = 0;
    ssize_t nread;

    struct movie* head = NULL;
    struct movie* tail = NULL;

    struct movie* prev_movie = NULL;

    // get rid of the first line, which is formatting and not any actual data
    getline(&current_line, &len, movie_file);

    // scrub through each line, creating a movie with all the info in it
    while ((nread = getline(&current_line, &len, movie_file)) != -1) {
        struct movie* new_movie_node = create_movie(current_line);

        // build up the doubly linked list
        if (head == NULL) {
            head = new_movie_node;
            tail = new_movie_node;
        } else {
            tail->next = new_movie_node;
            tail = new_movie_node;
            tail->prev = prev_movie;
        }

        prev_movie = new_movie_node;
    }

    free(current_line);
    fclose(movie_file);

    // return the head of the doubly linked list
    return head;
}

// get_list_size function
int get_list_size(struct movie* movie_list) {
    int counter = 0;
    while (movie_list != NULL)
    {
        // count up each movie
        counter ++;
        movie_list = movie_list->next;
    }

    // return total count
    return counter;
}

// get a list of movies based on the inputted year
struct movie* get_year_list(struct movie* movie_list, int input_year) {
    struct movie* head = NULL;
    struct movie* tail = NULL;

    struct movie* prev_movie = NULL;

    // cycle through each movie in the list
    while (movie_list != NULL) {
        // if the years match
        if (movie_list->year == input_year) {
            // incorporate it into a new doubly linked list
            struct movie* new_movie_node = malloc(sizeof(struct movie));

            new_movie_node->title = movie_list->title;
            new_movie_node->year = movie_list->year;
            new_movie_node->language_list = movie_list->language_list;
            new_movie_node->rating_value = movie_list->rating_value;
            new_movie_node->next = NULL;

            if (head == NULL) {
                head = new_movie_node;

                tail = new_movie_node;
            } else {
                tail->next = new_movie_node;
                tail = new_movie_node;
                tail->prev = prev_movie;
            }
        }
        prev_movie = movie_list;
        movie_list = movie_list->next;
    }

    // return the head of the new filtered doubly linked list
    return head;
}

// get a list of movies based on an inputted language
struct movie* get_movie_language_list(struct movie* movie_list, char* input_language) {
    struct movie* head = NULL;
    struct movie* tail = NULL;

    struct movie* prev_movie = NULL;

    // cycle through the doubly linked list of movies
    while (movie_list != NULL) {
        int has_language = 0;

        struct language* language_pointer = movie_list->language_list;

        // cycle through a movie's language list
        while(language_pointer != NULL) {
            if (strcmp(get_name(language_pointer), input_language) == 0) {
                // if the movie has the language, take note of it
                // note that strcmp is case sensitive
                has_language = 1;
            }
            language_pointer = get_next(language_pointer);
        }

        if (has_language) {
            // if the movie has a particular language, add it to a new doubly linked list
            struct movie* new_movie_node = malloc(sizeof(struct movie));

            new_movie_node->title = movie_list->title;
            new_movie_node->year = movie_list->year;
            new_movie_node->language_list = movie_list->language_list;
            new_movie_node->rating_value = movie_list->rating_value;
            new_movie_node->prev = NULL;
            new_movie_node->next = NULL;

            if (head == NULL) {
                head = new_movie_node;
                tail = new_movie_node;
            } else {
                tail->next = new_movie_node;
                tail = new_movie_node;
                tail->prev = prev_movie;
            }
        }
        prev_movie = movie_list;
        movie_list = movie_list->next;
    }

    // return the head of the new filtered doubly linked list
    return head;
}

// get a list of ranked movies for each distinct year
struct movie* get_ranked_list(struct movie* movie_list) {
    struct movie* head = NULL;
    struct movie* tail = NULL;

    // cycle through all the movies
    while (movie_list != NULL) {
        struct movie* temp_movie = head;

        int found_year = 0;

        // cycle through currently catalogued movies
        while (temp_movie != NULL) {
            // if the movie years match...
            if (temp_movie->year == movie_list->year) {
                found_year = 1;

                // check if the rating value is higher on the new movie
                if (temp_movie->rating_value < movie_list->rating_value) {
                    // if it is, replace it
                    struct movie* new_movie_node = malloc(sizeof(struct movie));

                    new_movie_node->prev = temp_movie->prev;
                    new_movie_node->next = temp_movie->next;

                    if (temp_movie->next != NULL) {
                        temp_movie->next->prev = new_movie_node;
                    }
                    if (temp_movie->prev != NULL) {
                        temp_movie->prev->next = new_movie_node;
                    }

                    new_movie_node->title = movie_list->title;
                    new_movie_node->year = movie_list->year;
                    new_movie_node->language_list = movie_list->language_list;
                    new_movie_node->rating_value = movie_list->rating_value;

                    if (temp_movie == head) {
                        head = new_movie_node;
                    }

                    if (temp_movie == tail) {
                        tail = new_movie_node;
                    }
                }
            }

            temp_movie = temp_movie->next;
        }

        if (found_year == 0) {
            if (head == NULL) {
                // if the list is empty, include the first movie encountered
                head = malloc(sizeof(struct movie));
                tail = head;

                head->next = NULL;
                head->prev = NULL;
                head->title = movie_list->title;
                head->year = movie_list->year;
                head->language_list = movie_list->language_list;
                head->rating_value = movie_list->rating_value;
            } else {
                // if the movie's year has not been incorporated yet, include the movie at the end of the list
                struct movie* new_movie_node = malloc(sizeof(struct movie));

                new_movie_node->title = movie_list->title;
                new_movie_node->year = movie_list->year;
                new_movie_node->language_list = movie_list->language_list;
                new_movie_node->rating_value = movie_list->rating_value;

                tail->next = new_movie_node;
                new_movie_node->prev = tail;
                new_movie_node->next = NULL;
                tail = new_movie_node;
            }
        }

        movie_list = movie_list->next;
    }
    
    // return the head of the newly created doubly linked list
    return head;
}

// return a movie's title
char* get_title(struct movie* movie) {
    return movie->title;
}

// return a movie's year
int get_year(struct movie* movie) {
    return movie->year;
}

// return a movie's rating value
float get_rating_value(struct movie* movie) {
    return movie->rating_value;
}

// return a movie struct pointer to the next movie in the doubly linked list
struct movie* get_next_movie(struct movie* movie) {
    return movie->next;
}