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

int explore_directory(int file, const char* dirPath, const char* filePath);

int main(int argc, char *argv[]){

	int file;

	if (argc != 3)
		fprintf(stderr, "Wrong number of arguments. Usage: %s <directory path> <save file path>.\n", argv[0]);

	if ((file = open(argv[2], O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, S_IRWXU)) == -1){
		fprintf(stderr, "Failed to create %s.", argv[2]);
		exit(1);
	}

	explore_directory(file, argv[1], argv[2]);

	close(file);

	return 0;
}

int explore_directory(int file, const char* dirPath, const char* filePath){

	struct dirent *dirStream;
	struct stat fileStat;
	pid_t pid;
	DIR * inDir;
	char path[BUF_LENGTH], buffer[BUF_LENGTH];
	int status;

	if((inDir = opendir(dirPath)) == NULL){
		fprintf(stderr, "Couldn't open the directory <%s>.\n", dirPath);
		exit(2);
	}

	while((dirStream = readdir(inDir)) != NULL){
		sprintf(path, "%s/%s", dirPath, dirStream->d_name);

		if (lstat(path, &fileStat) < 0){
			fprintf(stderr, "%s.\n", strerror(errno));
			closedir(inDir);
			exit(3);
		}

		// Verifies if is a regular file
		if(S_ISREG(fileStat.st_mode)){
			sprintf(buffer,"%-20s %40s %d %d %d\n", dirStream->d_name, path, (int) fileStat.st_mtime, (int) fileStat.st_size, (int) fileStat.st_mode);
			write(file, buffer, strlen(buffer));
		}

		// Avoids open current and parent directorys
		else if ((S_ISDIR(fileStat.st_mode)) && strcmp(dirStream->d_name,".")!=0
				&& strcmp(dirStream->d_name,"..")!=0){

			pid = fork();

			if(pid == -1){
				fprintf(stderr, "Fork failed!\n");
				exit(4);
			}else if (pid == 0) { // child
				explore_directory(file, path, filePath);
				exit(5);
			}

			else{ // parent
				if (waitpid(pid, &status, 0) == -1)
					fprintf(stderr, "%s.\n", strerror(errno));
			}
		}
	}

	closedir(inDir);

	return 0;
}
