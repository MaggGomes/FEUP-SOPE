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

const char* filePath = "./files.txt";
const char* hlinksPath = "./hlinks.txt";

typedef struct {
	char name[BUF_LENGTH];
	char path[BUF_LENGTH];
	char date[BUFINFO_LENGTH];
	char permissions[BUFINFO_LENGTH];
	char size[BUFINFO_LENGTH];
} fileInfo;

int sort_file(const char* fileName);

int check_dupfiles(const char* filePath);

int equals_files(fileInfo * file1, fileInfo * file2);

fileInfo* load_file(char* WfileString);

int main(int argc, char* argv[]) {
	// TODO - Numerar os exits de todas as funções
	check_dupfiles(filePath);
	pid_t pid = fork();
	int status;

	if (argc != 2 ){
		fprintf(stderr, "Invalid number of arguments. Usage: %s <directory path>\n", argv[0]);
		exit(1);
	}

	if (pid == -1) {
		fprintf(stderr, "Fork failed!\n");
		exit(2);
	}

	else if (pid == 0) { // Child
		// Starts a new process, executing lsdir program on it
		if (execlp("./lsdir", "lsdir", argv[1], filePath, NULL) == -1){
			fprintf(stderr, "execlp error: %s", strerror(errno));
			exit(3);
		}
	}
	else { // Parent
		if (waitpid(pid, &status, 0) == -1){
			fprintf(stderr, "%s.\n", strerror(errno));
			exit(4);
		}

		sort_file(filePath);

		//TODO para debugging - apagar
		/*char line[BUF_LENGTH];
		FILE* f= fopen(filePath, "r");
		fgets(line, BUF_LENGTH, f);
		fgets(line, BUF_LENGTH, f);
		fgets(line, BUF_LENGTH, f);
		fgets(line, BUF_LENGTH, f);
		fgets(line, BUF_LENGTH, f);

		fclose(f);
		load_file(line);*/
	}

	return 0;
}

int sort_file(const char* fileName)
{
	int n, fd[2];
	int file, status;
	char buffer[BUF_LENGTH];
	pid_t pid;

	if (pipe(fd) != 0) {
		fprintf(stderr, "Pipe error,\n");
		exit(1);
	}

	if ((pid = fork()) == -1) {
		fprintf(stderr, "Fork failed!\n");
		exit(2);
	}

	else if (pid == 0) { // Child
		close(fd[READ]); // Closes reading side

		if ((file = open(fileName, O_RDONLY, S_IRWXU)) == -1){
			fprintf(stderr, "Failed to open %s.", fileName);
			exit(3);
		}

		// Reads the information of each file and sends it through the pipe to the parent
		while ((n = read(file, buffer, BUF_LENGTH)) != 0) {
			if (write(fd[WRITE], buffer, n) != n) {
				fprintf(stderr, "write error to pipe\n");
				exit(4);
			}
		}

		close(fd[WRITE]); // Closes writing side
		close(file);
	}
	else { // Parent
		if (waitpid(pid, &status, 0) == -1){
			fprintf(stderr, "%s.\n", strerror(errno));
			exit(5);
		}

		close(fd[WRITE]); // Closes writing side
		dup2(fd[READ], STDIN_FILENO);
		close(fd[READ]); // Closes reading side

		if ((file = open(fileName, O_RDWR | O_CREAT , S_IRWXU)) == -1){
			fprintf(stderr, "Failed to create %s.", fileName);
			exit(6);
		}

		dup2(file, STDOUT_FILENO);
		if (execlp("sort", "sort", NULL) == -1){
			fprintf(stderr, "execlp error: %s", strerror(errno));
			exit(7);
		}

		close(file);
	}

	return 0;
}

int check_dupfiles(const char* filePath){
	fileInfo* temp;
	char line[256];
	FILE* f= fopen(filePath, "r");
	while (fgets(line, sizeof(line), f) != NULL) {
		printf("%s", line);
		temp = load_file(line);
	};
	fclose(f);

	/*
	if ((f = fopen(filePath, "r")) == NULL){
	fprintf(stderr, "Couldn't open %s.", filePath);
	fclose(f);
	exit(1);
}*/








	//fclose(f);

	return 0;
}

int equals_files(fileInfo * file1, fileInfo * file2){

	FILE *f1, *f2;
	char c1, c2;

	if (strcmp(file1->name, file2->name) != 0 || strcmp(file1->permissions, file2->permissions)!=0
	|| strcmp(file1->size, file2->size)!=0)
	return -1; // Files are different

	if ((f1 = fopen(file1->path, "r")) == NULL){
		fprintf(stderr, "Couldn't access %s.", file1->path);
		fclose(f1);
		exit(1);
	}

	if ((f2 = fopen(file2->path, "r")) == NULL){
		fprintf(stderr, "Couldn't access %s.", file2->path);
		fclose(f1);
		fclose(f2);
		exit(1);
	}

	while ((c1 = getc(f1)) != EOF || (c2 = getc(f2)) != EOF){
		if (c1 != c2)
		return -1; // Files's Content are different from each other
	}

	if (c1 != c2)
	return -1; // Files's Content are different from each other

	return 0;
}

fileInfo* load_file(char* WfileString){
	fileInfo* file = (fileInfo *) malloc(sizeof(2* BUF_LENGTH+ 3 * BUFINFO_LENGTH));
	int i = 0;
	const char space[2] = " \n";
	char *fileString = strtok(WfileString, space);

	strcpy(file->name, fileString);
	fileString  = strtok(NULL, space);
	while( fileString  != NULL )
	{
		if (i == 0)
		strcpy(file->path, fileString);
		else if (i == 1)
		strcpy(file->date, fileString);
		else if (i == 2)
		strcpy(file->size, fileString);
		else if (i == 3)
		strcpy(file->permissions, fileString);

		fileString  = strtok(NULL, space);
		i++;
	}

	// TODO - apagar - debugging
/*	printf("%s\n", file->name);
	printf("%s\n", file->path);
	printf("%s\n", file->date);*/
	printf("%s\n", file->size);
	//printf("%s\n", file->permissions);

	return file;
}
