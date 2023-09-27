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
    size_t nread;

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