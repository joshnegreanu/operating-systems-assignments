#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    char* file_name = argv[1];

    FILE* movie_file = fopen(file_name, "r");

    char* current_line = NULL;
    size_t len = 0;
    ssize_t nread;

    while (nread = getline(&current_line, &len, movie_file)) != -1) {
        struct movie* current_movie = malloc(sizeof(struct movie));

        char* token_pointer;

        //title
        char* token = strtok_r(current_line, ",", &token_pointer);
        current_movie->title = calloc(strlen(token) + 1, sizeof(char));
        strcpy(current_movie->title, token);

    }

    free(current_line);
    fclose(movie_file);
}

// movie structure
struct movie {
    char* title;
    int year;
    struct language* language_list;
    float rating_value;

    struct movie* next;
};