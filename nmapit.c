/* nmapit.c: Simple network port scanner */

#include "socket.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netdb.h>
#include <signal.h>

#ifndef GNU_SOURCE
typedef void (*sighandler_t)(int);
#endif

/* Functions */

/**
 * Display usage message and exit.
 * @param   status      Exit status
 **/
void    usage(int status) {
    fprintf(stderr, "Usage: nmapit [-p START-END] HOST\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "    -p START-END    Specifies the range of port numbers to scan\n");
    exit(status);
}

/**
 * Handle alarm signal.
 * @param   signum      Signal number
 **/
void sigalrm_handler(int signum) {
    // Cancel current alarm
    alarm(0);
}

/**
 * Parse port range string into start and end port integers.
 * @param   range       Port range string (ie. START-END)
 * @param   start       Pointer to starting port integer
 * @param   end         Pointer to ending port integer
 * @return  true if parsing both start and end were successful, otherwise false
 **/
bool parse_ports(char *range, int *start, int *end) {
    // Parse starting port
    char *token = strtok(range, "-");
    if (token == NULL)
        return false;
    *start = atoi(token);

    // Parse ending port
    token = strtok(NULL, "-");
    if (token == NULL)
        return false;
    *end = atoi(token);
    
    return true;
}

/**
 * Scan ports at specified host from starting and ending port numbers
 * (inclusive).
 * @param   host        Host to scan
 * @param   start       Starting port number
 * @param   end         Ending port number
 * @return  true if any port is found, otherwise false
 **/
bool scan_ports(const char* host, int start, int end) {
    // Register signal handler for alarm
    struct sigaction action = {.sa_handler = sigalrm_handler};
    sigaction(SIGALRM, &action, NULL);

    // For each port, set alarm, attempt to dial host and port
    char port_str[BUFSIZ];
    bool p = false;

    for (int i = start; i <= end; i++) {
        alarm(1);
        sprintf(port_str, "%d", i);

        if (socket_dial(host, port_str) != NULL) {
            alarm(0);
            printf("%s\n", port_str);
            p = true;
        }
    }
    
    if (p) return true;
    else return false;
}

/* Main Execution */

int main(int argc, char *argv[]) {
    // Parse command-line arguments
    if (argc < 2) usage(1);

    char *range = NULL;
    char *host = NULL;
    int start = 1;
    int end = 1023;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) usage(0);
        else if (strcmp(argv[i], "-p") == 0) {
            if (i+1 >= argc) usage(1);
            else {
                range = argv[i+1];
                i++;
                if (!parse_ports(range, &start, &end)) usage(EXIT_FAILURE);
            }
        } else {
            host = argv[i];
        }
    }

    // Scan ports
    if (scan_ports(host, start, end)) return EXIT_SUCCESS;
    return EXIT_FAILURE;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
