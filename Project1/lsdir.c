
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

int explore_directory(const char* dirPath, const char* filePath){

	struct dirent *dirStream;
	struct stat fileStat;
	pid_t pid;
	DIR * inDir;
	char path[BUF_LENGTH];
	int status;
	FILE * output;

	if((inDir = opendir(dirPath)) == NULL){
		fprintf(stderr, "Error:: Couldn't open the directory <%s>.\n", dirPath);
		exit(1);
	}

	while((dirStream = readdir(inDir)) != NULL){
		sprintf(path, "%s/%s", dirPath, dirStream->d_name);

		if (lstat(path, &fileStat) < 0){
			fprintf(stderr, "Error:: %s.\n", strerror(errno));
			closedir(inDir);
			exit(2);
		}

		if(S_ISREG(fileStat.st_mode)){
			output = fopen(filePath,"a");
			//name_of_the_file    complete_path    date_of_last_mod   size_in_bytes    mode_of_the_file
			fprintf(output,"%-20s %40s %d %d %d\n", dirStream->d_name, path, (int) fileStat.st_mtime, (int) fileStat.st_size, (int) fileStat.st_mode);
			fclose(output);
		}


		// Avoids open current and parent directorys
		else if ((S_ISDIR(fileStat.st_mode)) && strcmp(dirStream->d_name,".")!=0
				&& strcmp(dirStream->d_name,"..")!=0){

			pid = fork();

			if(pid == -1){
				fprintf(stderr, "Error: Fork failed!\n");
				exit(3);
			}else if (pid == 0) { // child
				explore_directory(path, filePath);
				exit(4);
			}

			else{ // parent
				if (waitpid(pid, &status, 0) == -1)
					fprintf(stderr, "Error:: %s.\n", strerror(errno));
			}
		}
	}

	closedir(inDir);

	return 0;
}

int main(int argc, char *argv[]){

	if (argc != 3)
		fprintf(stderr, "ERROR:: Wrong number of arguments. Usage: %s <directory path> <save file path>.\n", argv[0]);

	// Clears the file where the files will be saved
	open(argv[2], O_TRUNC);
	explore_directory(argv[1], argv[2]);

	return 0;
}
