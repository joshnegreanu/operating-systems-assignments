#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "language.h"

struct language {
    char* name;
    struct language* next;
};

// create a language struct given a string name input
struct language* create_language(char* language_name) {
    struct language* current_language = malloc(sizeof(struct language));
    current_language->name = calloc(strlen(language_name) + 1, sizeof(char));
    
    // provide the name of the language
    strcpy(current_language->name, language_name);
    current_language->next = NULL;

    return current_language;
}

// return the head of the language linked list
struct language* get_language_list(char* current_language_line) {
    // break down a string using ; delimeter into a linked list
    int len = strlen(current_language_line); 
    if(len > 0)
        current_language_line++;
    if(len > 1)
        current_language_line[len - 2] = '\0';

    struct language* head = NULL;
    struct language* tail = NULL;

    char* token = strtok(current_language_line, ";");

    // scrub through the entire string to create the linked list
    while(token != NULL)
	{
        struct language* new_language_node = create_language(token);

        if (head == NULL) {
            head = new_language_node;
            tail = new_language_node;
        } else {
            tail->next = new_language_node;
            tail = new_language_node;
        }
		
        token = strtok(NULL, ";");
	}

    // return head of the list
    return head;
}

// return the name of the language
char* get_name(struct language* language) {
    return language->name;
}

// return the langauge struct pointer that's after the current language in the list
struct language* get_next(struct language* language) {
    return language->next;
}