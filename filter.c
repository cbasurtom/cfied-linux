/* filter.c: Filter functions */

#include "findit.h"

#include <stdlib.h>
#include <string.h>

#include <fnmatch.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/* Filter Functions */

/**
 * Determines if file at specified path has matching file type.
 * @param   path        Path string
 * @param   options     Pointer to options structure
 * @return  true if file at specified path has matching file type specified in
 * options.
 **/
bool	filter_by_type(const char *path, Options *options) {

    struct stat fs;
    if (lstat(path, &fs) == 0) {
        if ((fs.st_mode & S_IFMT) == options->type) return true;
    }
    return false;
}

/**
 * Determines if file at specified path has matching basename.
 * @param   path        Path string
 * @param   options     Pointer to options structure
 * @return  true if file at specified path has basename that matches specified
 * pattern in options.
 **/
bool	filter_by_name(const char *path, Options *options) {

    if (fnmatch(options->name, basename((char*)path), 0) == 0) {
        return true;
    }
    return false;
}

/**
 * Determines if file at specified path has matching access mode.
 * @param   path        Path string
 * @param   options     Pointer to options structure
 * @return  true if file at specified path has matching access mode specified
 * in options.
 **/
bool	filter_by_mode(const char *path, Options *options) {

    if (access(path, options->mode) == 0) return true;

    return false;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
