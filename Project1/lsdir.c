
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#define BUF_LENGTH 512

int main(int argc, char *argv[]){
	pid_t pid;
	DIR * inDir = opendir(argv[1]);
	struct dirent *tempDir	= readdir(inDir);
	struct stat stat_buf;
	char diretorio[BUF_LENGTH];

	if(argc != 2){
		printf("Invalid Arguments");
		return 1;
	}

	if(inDir == NULL){
		printf("Invalid Directory");
		return 1;
	}

	while((tempDir = readdir(inDir)) != NULL){
		sprintf(diretorio, "%s/%s", argv[1], tempDir->d_name);

		if (lstat(diretorio, &stat_buf) < 0){
			perror(tempDir->d_name);
      closedir(inDir);
      return 1;
    }

		if(S_ISREG(stat_buf.st_mode)){

			printf("%s\n",diretorio);

		}else if ((S_ISDIR(stat_buf.st_mode)) && strcmp(tempDir->d_name,".") && strcmp(tempDir->d_name,"..")){

			pid = fork();

			if(pid < 0){
				printf("Fork failed");
				return 1;
			}else if (pid == 0) {

				printf("%s\n",diretorio);
				execl("lsdir", "lsdir", (diretorio), NULL);
				closedir(inDir);
				return 0;
			}
		}
	}

	closedir(inDir);
	return 0;
}
