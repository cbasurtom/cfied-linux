/* str.c: string library */

#include "str.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

/* Functions */

/**
 * Convert string to lowercase.
 * @param   s	    String to convert
 * @param   w	    Pointer to buffer that holds result of conversion
 **/
void	str_lower(const char *s, char *w) {   
    while (*s != '\0') {
        *w = tolower(*s);
        s++;
        w++;
    }
    *w = '\0';
}

/**
 * Convert string to uppercase.
 * @param   s	    String to convert
 * @param   w	    Pointer to buffer that holds result of conversion
 **/
void	str_upper(const char *s, char *w) {
    while (*s != '\0') {
        *w = toupper(*s);
        s++;
        w++;
    }
    *w = '\0';
}

/**
 * Convert string to titlecase.
 * @param   s	    String to convert
 * @param   w	    Pointer to buffer that holds result of conversion
 **/
void	str_title(const char *s, char *w) {

    int bound = 1; 
    while (*s != '\0') {
        if (isalpha(*s) != 0) {
            if (bound == 1) {
                *w = toupper(*s);
                bound = 0;
            } else *w = tolower(*s);
        } else {
            *w = *s;
            bound = 1;
        }
        s++;
        w++;
    }
    *w = '\0';
}

/**
 * Strip characters from back of string (if present).
 * @param   s	    String to strip
 * @param   chars   Characters to strip (if NULL, then all whitespace)
 * @param   w	    Pointer to buffer that holds result of strip
 **/
void	str_rstrip(const char *s, const char *chars, char *w) {
    
    if (chars == NULL) chars = " \t\n\r\v\f";

    int deletes[1<<8] = {0};

    for (const char *c = chars; *c; c++) {
        deletes[(int)*c]++;
    }

    char *head = w;

    while (*s != '\0') {
        *w = *s;
        s++;
        w++;
    }

    *w = '\0';
    
    w--;

    while (w >= head && deletes[(int)*w] != 0) {
        *w = '\0';
        w--;
    }
}

/**
 * Delete characters from string.
 * @param   s	    String to delete from
 * @param   chars   Characters to delete
 * @param   w	    Pointer to buffer that holds result of deletion
 **/
void	str_delete(const char *s, const char *chars, char *w) {

    int deletes[1<<8] = {0};

    for (const char *c = chars; *c; c++) {
        deletes[(int)*c]++;
    }

    while (*s != '\0') { 
        if (deletes[(int)*s] == 0) {
            *w = *s;
            w++;
        }
        s++;
    }

    *w = '\0';
}

/**
 * Translate characters in 'from' with corresponding characters in 'to'.
 * @param   s       String to translate
 * @param   from    String with characters to translate
 * @param   to      String with corresponding translation characters
 * @param   w	    Pointer to buffer that holds result of translation
 **/
void	str_translate(const char *s, const char *from, const char *to, char *w) {

    if ((*from == '\0') || (*to == '\0')) {
        while(*s != '\0') {
            *w = *s;
            w++;
            s++;
        }
        *w = '\0';
        return;
    }
    
    int translates[1<<8] = {0};

    for (const char *c = from; *c; c++) {
        translates[(int)*c] = *to;
        to++;
    }

    while (*s != '\0') { 
        if (translates[(int)*s] != 0) {
            *w = (char)translates[(int)*s];
        } else *w = *s;
        w++;
        s++;
    }

    *w = '\0';
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
