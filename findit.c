/* findit.c: Search for files in a directory hierarchy */

#include "findit.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <unistd.h>

/* Macros */

#define	streq(a, b) (strcmp(a, b) == 0)

/* Functions */

/**
 * Print usage message and exit with status
 * @param   status      Exit status
 **/
void usage(int status) {
    fprintf(stderr, "Usage: findit PATH [OPTIONS]\n\n");
    fprintf(stderr, "Options:\n\n");
    fprintf(stderr, "   -type [f|d]	File is of type f for regular file or d for directory\n");
    fprintf(stderr, "   -name pattern	Name of file matches shell pattern\n");
    fprintf(stderr, "   -executable	File is executable or directory is searchable by user\n");
    fprintf(stderr, "   -readable	File is readable by user\n");
    fprintf(stderr, "   -writable	File is writable by user\n");
    exit(status);
}

/**
 * Recursively walk specified directory, adding all file system entities to
 * specified files list.
 * @param   root        Directory to walk
 * @param   files       List of files found
 **/
void	find_files(const char *root, List *files) {
    // Add root to files
    
    list_append(files, (Data)strdup(root));
    
    DIR *d = opendir(root);

    if (!d) return;

    // Walk directory
    for (struct dirent *e = readdir(d); e; e = readdir(d)) {
        // Skip current and parent directory entries
        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0) continue;

        //  Form full path to entry
        char path[BUFSIZ];
        sprintf(path, "%s/%s", root, e->d_name);

        // Recursively walk directories or add entry to files list
        if (e->d_type == DT_DIR) find_files(path, files);
        else list_append(files, (Data)strdup(path));    
    }

    closedir(d);

}

/**
 * Iteratively filter list of files with each filter in list of filters.
 * @param   files       List of files
 * @param   filters     List of filters
 * @param   options     Pointer to options structure
 **/
void	filter_files(List *files, List *filters, Options *options) {
    // Apply each filter to list of files
    for (Node *curr = filters->head; curr; curr = curr->next){
        list_filter(files, curr->data.function, options, true);
    }
        
}

/* Main Execution */

int main(int argc, char *argv[]) {
    // Parse command line arguments */
    char root[BUFSIZ] = ".";
    char buff[BUFSIZ] = {0};
    
    List files = {0};
    List filters = {0};
    Options o = {0};
    o.name = buff;
    
    if (argc == 1) usage(1);

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-type") == 0) {
            if (argc > i+1) {
                if (strcmp(argv[i+1], "d") == 0) {
                    o.type = S_IFDIR; 
                } else if (strcmp(argv[i+1], "f") == 0) o.type = S_IFREG;
                i++;
                list_append(&filters, (Data)filter_by_type);
            } else usage(1);
            
        }
        else if (strcmp(argv[i], "-name") == 0) {
            if (argc > i+1) {
                strcpy(o.name, argv[i+1]);
                i++;
                list_append(&filters, (Data)filter_by_name);
            } else usage(1);
        }
        else if (strcmp(argv[i], "-executable") == 0) {
            o.mode = X_OK;
            list_append(&filters, (Data)filter_by_mode);
        }
        else if (strcmp(argv[i], "-readable") == 0) {
            o.mode = R_OK;
            list_append(&filters, (Data)filter_by_mode);
        }
        else if (strcmp(argv[i], "-writable") == 0) {
            o.mode = W_OK;
            list_append(&filters, (Data)filter_by_mode);
        }
        else strcpy(root, argv[i]);
    }

    // Find files, filter files, print files
    find_files(root, &files);
    filter_files(&files, &filters, &o);
    list_output(&files, stdout);

    node_delete(files.head, true, true);
    node_delete(filters.head, false, true);

    return EXIT_SUCCESS;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
