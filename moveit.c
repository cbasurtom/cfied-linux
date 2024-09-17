/* moveit.c: Interactive Move Command */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

/* Macros */

#define streq(a, b) (strcmp(a, b) == 0)
#define strchomp(s) (s)[strlen(s) - 1] = 0

/* Functions */

/**
 * Display usage message and exit.
 * @param   status      Exit status.
 **/
void    usage(int status) {
    fprintf(stderr, "Usage: moveit files...\n");
    exit(status);
}

/**
 * Save list of file paths to temporary file.
 * @param   files       Array of path strings.
 * @param   n           Number of path strings.
 * @return  Newly allocated path to temporary file (must be freed).
 **/
char *  save_files(char **files, size_t n) {
    // Create temporary file
    char tpath[] = "moveit.XXXXXX";
    int tfd = mkstemp(tpath);

    if (tfd < 0) {
        fprintf(stderr, "Unable to mkstemp: %s\n", strerror(errno));
        return NULL;
    }

    // Open temporary file for writing
    FILE *fp = fdopen(tfd, "w");

    if (fp == NULL) {
        fprintf(stderr, "Unable to open file: %s\n", strerror(errno));
        close(tfd);
        return NULL;
    }

    // Write paths to temporary file
    for (size_t i = 0; i < n; i++) {
        fprintf(fp, "%s\n", files[i]);
    }

    fclose(fp);

    return strdup(tpath);
}

/**
 * Run $EDITOR on specified path.
 * @param   path        Path to file to edit.
 * @return  Whether or not the $EDITOR process terminatesd successfully.
 **/
bool    edit_files(const char *path) {
    // Get EDITOR from environment (default to vim if not found)
    char *editor = getenv("EDITOR");
    if (editor == NULL) editor = "vim";

    // Fork process
    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Unable to fork: %s\n", strerror(errno));
        return false;
    }
    //  1. Child: execute editor on path
    else if (pid == 0) {
        execlp(editor, editor, path, NULL);
        fprintf(stderr, "Unable to execl: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    //  2. Parent: wait for child
    else {
        int status;
        wait(&status);
        // Return exit status of child process
        if (WIFEXITED(status)) return WEXITSTATUS(status) == EXIT_SUCCESS;
        else return false;
    }
}

/**
 * Rename files as specified in contents of path.
 * @param   files       Array of old path names.
 * @param   n           Number of old path names.
 * @param   path        Path to file with new names.
 * @return  Whether or not all rename operations were successful.
 **/
bool    move_files(char **files, size_t n, const char *path) {
    // Open temporary file at path for reading
    FILE *fp = fopen(path, "r");
    
    if (fp == NULL) {
        fprintf(stderr, "Unable to open file: %s\n", strerror(errno));
        return false;
    }

    // Rename each file in array according to new name in temporary file
    // (if the names do not match)
    char token[BUFSIZ];
    size_t i = 0;

    while(fgets(token, BUFSIZ, fp) != NULL) {
        strchomp(token);
        if (!streq(files[i],token)) {
            int rst = rename(files[i],token);
            if (rst < 0) {
                fprintf(stderr, "Unable to rename file: %s\n", strerror(errno));
                fclose(fp);
                return false;
            }
        }
        i++;
        if (i == n) break;
    }

    fclose(fp);

    return true;
}

/* Main Execution */

int     main(int argc, char *argv[]) {
    // Parse command line options
    if (argc == 1) usage(1);

    if (streq(argv[1], "-h")) usage(0);

    char *path = save_files(&argv[1], argc-1);

    if (!edit_files(path)) usage(1);

    if (!move_files(&argv[1], argc-1, path)) usage(1);

    // Cleanup temporary file
    int cstatus = unlink(path);
    if (cstatus < 0) {
        fprintf(stderr, "Unable to unlink: %s\n", strerror(errno));
        free(path);
        return EXIT_FAILURE;
    }
    
    free(path);

    return 0;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
