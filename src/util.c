#define _POSIX_C_SOURCE 200809L
#include "util.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define mkdir(dir, mode) _mkdir(dir)
#else
#include <unistd.h>
#endif

int ensure_directory_exists(const char *dir_path) {
    struct stat st = {0};
    if (stat(dir_path, &st) == -1) {
        if (mkdir(dir_path, 0755) != 0) {
            return -1;
        }
    }
    return 0;
}
