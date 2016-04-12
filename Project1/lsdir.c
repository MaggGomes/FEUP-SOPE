
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

	printf("%s\n", dirPath);
	if((inDir = opendir(dirPath)) == NULL){
		fprintf(stderr, "Error:: Couldn't open the directory <%s>.\n", dirPath);
		return 1;
	}

	printf("entrou\n");

	while((dirStream = readdir(inDir)) != NULL){
		sprintf(path, "%s/%s", dirPath, dirStream->d_name);

		if (lstat(path, &fileStat) < 0){
			fprintf(stderr, "Error:: %s.\n", strerror(errno));
			closedir(inDir);
			return 2;
		}

		if(S_ISREG(fileStat.st_mode)){
			output = fopen(filePath,"a");
			fprintf(output,"%s   %s\n",dirStream->d_name,path);
			fclose(output);
		}


		// Avoids open current and parent directorys
		else if ((S_ISDIR(fileStat.st_mode)) && strcmp(dirStream->d_name,".")!=0
				&& strcmp(dirStream->d_name,"..")!=0){

			pid = fork();

			if(pid == -1){
				fprintf(stderr, "Error: Fork failed!\n");
				return 3;
			}else if (pid == 0) { // child
				explore_directory(path, filePath);
				return 4;
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

	// TODO decidir se no ficheiros.txt se imprime o path das pastas ou se apenas dos ficheiros

	if (argc != 3)
		fprintf(stderr, "ERROR:: Wrong number of arguments. Please call as follows: "
				"lsdir <directory path> <save file path>.\n");

	// Clears the file where the files will be saved
	open(argv[2], O_TRUNC);
	explore_directory(argv[1], argv[2]);
	printf("%s\n%s\n", argv[1], argv[2]);

	return 0;
}
