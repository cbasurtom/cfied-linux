/* trit.c: translation utility */

#include "str.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Constants */

enum {
    LOWER   = 1<<1,
    UPPER   = 1<<2,    
    TITLE   = 1<<3,   
    STRIP   = 1<<4,   
    DELETE  = 1<<5,  
    INIT    = 1<<6,
};

#define WHITESPACE  " \t\r\n"

/* Functions */

void usage(int status) {
    fprintf(stderr, "Usage: trit SET1 SET2\n\n");
    fprintf(stderr, "Post Translation filters:\n\n");
    fprintf(stderr, "   -l      Convert to lowercase\n");
    fprintf(stderr, "   -u      Convert to uppercase\n");
    fprintf(stderr, "   -t      Convert to titlecase\n");
    fprintf(stderr, "   -s      Strip trailing whitespace\n");
    fprintf(stderr, "   -d      Delete letters in SET1\n");
    exit(status);
}

void translate_stream(FILE *stream, const char *set1, const char *set2, int flags) {

    char buffer[BUFSIZ];
    while (fgets(buffer, BUFSIZ, stdin)) {
        if (flags & DELETE) str_delete(buffer, set1, buffer);
        else if (flags & INIT) str_translate(buffer, set1, set2, buffer);
        else printf("");

        if (flags & LOWER) str_lower(buffer, buffer);
        if (flags & UPPER) str_upper(buffer, buffer);
        if (flags & TITLE) str_title(buffer, buffer);
        if (flags & STRIP) {
            str_rstrip(buffer, NULL, buffer);
            buffer[strlen(buffer)] = '\n';
        }
        
        printf("%s", buffer);
    }
}

/* Main Execution */

int main(int argc, char *argv[]) {
    // Parse command line arguments
    int flags = 0;
    int init = 0;
    char set1[BUFSIZ];
    char set2[BUFSIZ];

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) flags |= LOWER;
        else if (strcmp(argv[i], "-u") == 0) flags |= UPPER;
        else if (strcmp(argv[i], "-t") == 0) flags |= TITLE;
        else if (strcmp(argv[i], "-s") == 0) flags |= STRIP;
        else if (strcmp(argv[i], "-d") == 0) {
            flags |= DELETE;
            strcpy(set1, argv[i+1]);
            i++;
        } else if (strcmp(argv[i], "-h") == 0) usage(0);
        else {
            strcpy(set1, argv[i]);
            i++;
            strcpy(set2, argv[i]);
            flags |= INIT;
        }
    }

    translate_stream(stdin, set1, set2, flags);
    
    return EXIT_SUCCESS;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */

