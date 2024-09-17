/* curlit.c: Simple HTTP client*/

#include "socket.h"

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <netdb.h>

/* Constants */

#define HOST_DELIMITER  "://"
#define PATH_DELIMITER  '/'
#define PORT_DELIMITER  ':'
#define BILLION         (1000000000.0)
#define MEGABYTES       (1<<20)

/* Macros */

#define streq(a, b) (strcmp(a, b) == 0)

/* Structures */

typedef struct {
    char host[NI_MAXHOST];
    char port[NI_MAXSERV];
    char path[PATH_MAX];
} URL;

/* Functions */

/**
 * Display usage message and exit.
 * @param   status      Exit status.
 **/
void    usage(int status) {
    fprintf(stderr, "Usage: curlit [-h] URL\n");
    exit(status);
}

/**
 * Parse URL string into URL structure.
 * @param   s       URL string
 * @param   url     Pointer to URL structure
 **/
void    parse_url(const char *s, URL *url) {
    // Copy data to local buffer
    char buffer[BUFSIZ];
    strncpy(buffer, s, BUFSIZ);

    // Skip scheme to host
    char *scheme = strstr(buffer, "://");
    if (scheme == NULL) {
       scheme = buffer;
    } else {
        scheme += 3;
    }

    // Split host:port from path
    char *urlpath;
    urlpath = strstr(scheme, "/");

    if (urlpath == NULL) urlpath = "";
    else {
        *urlpath = '\0';
        urlpath++;
    }

    // Split host and port
    char *urlport;
    urlport = strstr(scheme, ":");

    if (urlport == NULL) urlport = "80";
    else {
        *urlport = '\0';
        urlport++;
    }

    // Copy components to URL
    strcpy(url->host, scheme);
    strcpy(url->port, urlport);
    strcpy(url->path, urlpath);

}

/**
 * Fetch contents of URL and print to standard out.
 *
 * Print elapsed time and bandwidth to standard error.
 * @param   s       URL string
 * @param   url     Pointer to URL structure
 * @return  true if client is able to read all of the content (or if the
 * content length is unset), otherwise false
 **/
bool    fetch_url(URL *url) {
    // Grab start time
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Connect to remote host and port
    FILE *client_file = socket_dial(url->host, url->port);
    if (!client_file) {
        return false;
    }

    // Send request to server
    fprintf(client_file, "GET /%s HTTP/1.0\r\n", url->path);
    fprintf(client_file, "Host: %s\r\n", url->host);
    fprintf(client_file, "\r\n");

    // Read status response from server
    char buffer[BUFSIZ];

    bool con = true;
    fgets(buffer, BUFSIZ, client_file);
    if (strstr(buffer, "200 OK") == NULL) {
        con = false;
    }

    // Read response headers from server
    int content_length = 0;
    while ((fgets(buffer, BUFSIZ, client_file)) && strlen(buffer) > 2) {
        if (strstr(buffer, "Content-Length:")) {
            sscanf(buffer, "Content-Length: %d", &content_length);
        }
    }

    // Read response body from server
    size_t nread            = 0;
    size_t total_bytes_read = 0;
    while ((nread = fread(buffer, 1, BUFSIZ, client_file)) > 0) {
        fwrite(buffer, 1, nread, stdout);
        total_bytes_read += nread;
    }

    // Grab end time
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / BILLION;

    // Output metrics
    fprintf(stderr, "Elapsed Time: %.2f s\n", elapsed_time);
    fprintf(stderr, "Bandwidth:    %.2f MB/s\n", total_bytes_read / (MEGABYTES*elapsed_time));

    fclose(client_file);
    
    if ((con == false) || ((content_length != 0) && (content_length != total_bytes_read)))  return false;
    else if ((con == true) && ((content_length == 0) || (content_length == total_bytes_read))) return true;

    return true;
}

/* Main Execution */

int     main(int argc, char *argv[]) {
    if (argc != 2) usage(1);

    URL url;
    char *s;
    
    // Parse command line options
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) usage(0);
        else if (argv[i][0] == '-') usage(1);
        else {
            s = argv[i];
        }
    }
 
    parse_url(s, &url);

    if (!fetch_url(&url)) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
