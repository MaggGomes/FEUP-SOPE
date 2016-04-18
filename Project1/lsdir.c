#include "lsdir.h"

int main(int argc, char *argv[]){

	int f;

	if (argc != 3)
		fprintf(stderr, "Wrong number of arguments. Usage: %s <directory path> <save file path>.\n", argv[0]);

	if ((f = open(argv[2], O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, S_IRWXU)) == -1){
		fprintf(stderr, "Failed to create %s.\n", argv[2]);
		exit(1);
	}

	explore_directory(f, argv[1]);

	close(f);

	return 0;
}

int explore_directory(int f, const char* dirPath){

	struct dirent *dirStream;
	struct stat fileStat;
	pid_t pid;
	DIR * inDir;
	char path[BUF_LENGTH], buffer[BUF_LENGTH];
	int status;

	if((inDir = opendir(dirPath)) == NULL){
		fprintf(stderr, "Couldn't open the directory <%s>.\n", dirPath);
		exit(1);
	}

	while((dirStream = readdir(inDir)) != NULL){
		sprintf(path, "%s/%s", dirPath, dirStream->d_name);

		if (lstat(path, &fileStat) < 0){
			fprintf(stderr, "%s.\n", strerror(errno));
			closedir(inDir);
			exit(2);
		}

		// Verifies if is possible to access the directory specified
		if (access(path, R_OK) != 0)
        {
            fprintf(stderr, "%s.\n", strerror(errno));
        }

		// Verifies if it's a regular file
		else if(S_ISREG(fileStat.st_mode)){
			// Name - date - path - size - permissions
			sprintf(buffer,"%s| %d %d %d %-40s\n", dirStream->d_name, (int) fileStat.st_mtime, (int) fileStat.st_size, (int) fileStat.st_mode, path);
			write(f, buffer, strlen(buffer));
		}

		// Avoids open current and parent directorys
		else if ((S_ISDIR(fileStat.st_mode)) && strcmp(dirStream->d_name,".")!=0
				&& strcmp(dirStream->d_name,"..")!=0){

			pid = fork();

			if(pid == -1){
				fprintf(stderr, "Fork failed!\n");
				exit(3);
			}else if (pid == 0) { // Child
				explore_directory(f, path);
				exit(0);
			}

			else{ // Parent
				if (waitpid(pid, &status, 0) == -1){
					fprintf(stderr, "%s.\n", strerror(errno));
					exit(4);
				}
			}
		}
	}

	closedir(inDir);

	return 0;
}
