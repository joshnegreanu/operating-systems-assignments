#ifndef __MOVIE_H
#define __MOVIE_H

// movie structure declaration
struct movie;

// function declarations
struct movie* create_movie(char* current_line);
struct movie* parse_file(char *file_name);

int get_list_size(struct movie* movie_list);

char* get_title(struct movie* movie);
int get_year(struct movie* movie);
float get_rating_value(struct movie* movie);
struct movie* get_next_movie(struct movie* movie);

#endif