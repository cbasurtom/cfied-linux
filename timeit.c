/* timeit.c: Run command with a time limit */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <fcntl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

/* Macros */

#define	streq(a, b) (strcmp(a, b) == 0)
#define strchomp(s) (s)[strlen(s) - 1] = 0
#define debug(M, ...) \
    if (Verbose) { \
        fprintf(stderr, "%s:%d:%s: " M, __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    }

#define BILLION 1000000000.0

/* Globals */

int  Timeout  = 10;
bool Verbose  = false;
int  ChildPid = 0;

/* Functions */

/**
 * Display usage message and exit.
 * @param   status      Exit status.
 **/
void	usage(int status) {
    fprintf(stderr, "Usage: timeit [options] command...\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "    -t SECONDS  Timeout duration before killing command (default is %d)\n", Timeout);
    fprintf(stderr, "    -v          Display verbose debugging output\n");
    exit(status);
}

/**
 * Parse command line options.
 * @param   argc        Number of command line arguments.
 * @param   argv        Array of command line argument strings.
 * @return  Array of strings representing command to execute (must be freed).
 **/
char ** parse_options(int argc, char **argv) {
    // Iterate through command line arguments to determine Timeout and
    // Verbose flags

    int i = 1;

    for (; i < argc; i++) {
        if (streq(argv[i],"-t")) {
            if (argc > i+1) {
                i++;
                Timeout = atoi(argv[i]);
            } else usage(1);
        } else if (streq(argv[i], "-v")) {
            Verbose = true;
        } else break;
    }

    debug("Timeout = %d\n", Timeout);
    debug("Verbose = %d\n", Verbose);

    // Copy remaining arguments into new array of strings
    char **command = calloc(argc-i+1, sizeof(char*));

    if (i >= argc) usage(1);

    for (int j = 0; j < argc-i; j++) {
        command[j] = argv[i+j];
    }

    command[argc-i] = NULL;

    if (Verbose) {
        // Print out new array of strings (to stderr)
        debug("Command =");
        for (int k = 0; command[k] != NULL; k++) printf(" %s", command[k]);
        printf("\n");
    }

    return command;
}

/**
 * Handle signal.
 * @param   signum      Signal number.
 **/
void    handle_signal(int signum) {
    // Kill child process
    debug("Killing child %d...\n", ChildPid);
    kill(ChildPid, SIGKILL);
}

/* Main Execution */

int	main(int argc, char *argv[]) {
    // Parse command line options
    if (argc == 1) usage(1);

    if (streq(argv[1], "-h")) usage(0);

    char **command = parse_options(argc, argv);

    // Register alarm handler and save start time
    debug("Registering handlers...\n");
    signal(SIGALRM, handle_signal);

    debug("Grabbing start time...\n");
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Fork child process:
    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Unable to fork: %s\n", strerror(errno));
        free(command);
        exit(EXIT_FAILURE);
    }

    //  1. Child executes command parsed from command line
    debug("Executing child...\n");
    if (pid == 0) {
        execvp(command[0], command);
        fprintf(stderr, "Unable to execvp: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    //  2. Parent sets alarm based on Timeout and waits for child
    else { 
        alarm(Timeout);
        
        ChildPid = pid;

        int status;
        wait(&status);

        debug("Sleeping for %d seconds...\n", Timeout);
        debug("Waiting for child %d...\n", ChildPid);

        debug("Child exit status: %d\n", status);

        debug("Grabbing end time...\n");
        struct timespec end_time;
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        
        double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / BILLION;

        printf("Time Elapsed: %0.1lf\n", elapsed_time);

        // Cleanup
        free(command);

        status = WIFEXITED(status) ? WEXITSTATUS(status) : WTERMSIG(status);

        return status;
    }

    free(command);

}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
