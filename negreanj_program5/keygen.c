#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]) {
    time_t t;
    srand((unsigned) time(&t));

    char* alphabet_string = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

    int i;
    for (i = 0; i < atoi(argv[1]); i ++) {
        printf("%c", alphabet_string[rand() % 27]);
    }

    return 0;
}