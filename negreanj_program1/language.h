#ifndef __LANGUAGE_H
#define __LANGUAGE_H

// language structure declaration
struct language;

// unction declarations
struct language* create_language(char* language_name);
struct language* get_language_list(char* current_language_line);

char* get_name(struct language* language);
struct language* get_next(struct language* language);

#endif