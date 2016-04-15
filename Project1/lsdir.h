#ifndef LSDIR_H
#define LSDIR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BUF_LENGTH 512

/**
 * @brief Explores the directory provided
 *
 * @param f file where the files's information will be saved
 * @param dirPath current directory being explored
 * @return 0 if no error occurred
 */
int explore_directory(int f, const char* dirPath);

#endif // LSDIR_H
