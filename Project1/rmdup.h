#ifndef RMDUP_H
#define RMDUP_H

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

#define READ 0
#define WRITE 1
#define BUF_LENGTH 512
#define BUFINFO_LENGTH 20

typedef struct {
	char name[BUF_LENGTH];
	char path[BUF_LENGTH];
	char date[BUFINFO_LENGTH];
	char permissions[BUFINFO_LENGTH];
	char size[BUFINFO_LENGTH];
	int isDupFile; // 1 if is a duplicated file, 0 otherwise
} fileInfo;

/**
 * @brief Sorts the file's directories saved
 *
 * @param fileName file to be sorted
 * @return 0 if no error occurred
 */
int sort_file(const char* fileName);

/**
 * @brief Verifies if there are duplicated files and created hard links for them
 *
 * @param filePath file where the file's directories are saved
 * @param directory which was explored
 * @return 0 if no error has occurred
 */
int check_dupfiles(const char* filePath, char* directory);

/**
 * @brief Loads the information of a file
 *
 * @param fileString from where the file information will be extracted
 * @return fileInfo returns a struct with the information stored
 */
fileInfo load_file(char* fileString);

/**
 * @brief Verifies if two files are equals
 *
 * @param file1 to be compared
 * @param file2 to be compared
 * @return 0 if the files are equals
 */
int equals_files(fileInfo * file1, fileInfo * file2);

#endif // RMDUP_H
