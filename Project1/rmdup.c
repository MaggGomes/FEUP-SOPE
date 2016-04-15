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

fileInfo load_file(char* WfileString);

int main(int argc, char* argv[]) {
	// TODO - Numerar os exits de todas as funções
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
		char line[BUF_LENGTH];
		FILE* f= fopen(filePath, "r");
		fgets(line, BUF_LENGTH, f);
		fgets(line, BUF_LENGTH, f);
		fgets(line, BUF_LENGTH, f);
		fgets(line, BUF_LENGTH, f);
		fgets(line, BUF_LENGTH, f);
		fgets(line, BUF_LENGTH, f);
		fgets(line, BUF_LENGTH, f);
		fgets(line, BUF_LENGTH, f);
		fgets(line, BUF_LENGTH, f);

		fileInfo a = load_file(line);
		fgets(line, BUF_LENGTH, f);
		fileInfo b = load_file(line);
		printf("%s\n", a.path);
		printf("%s\n", b.path);
		if (equals_files(&a, &b) == 0)
			printf("sim\n");
		else
			printf("nao\n");





		fclose(f);
		check_dupfiles(filePath);
	}

	return 0;
}

int sort_file(const char* fileName){
	int n, fd[2];
	int f, status;
	char buffer[BUF_LENGTH];
	pid_t pid, pidSort;

	// Creates a pipe to be used between the parent and child processes
	if (pipe(fd) != 0) {
		fprintf(stderr, "Pipe error,\n");
		exit(1);
	}

	if ((pid = fork()) == -1) { // Error
		fprintf(stderr, "Fork failed!\n");
		exit(2);
	}

	else if (pid == 0) { // Child
		close(fd[READ]); // Closes reading side

		if ((f = open(fileName, O_RDONLY, S_IRWXU)) == -1){
			fprintf(stderr, "Failed to open %s.", fileName);
			exit(3);
		}

		// Reads the information of each file and sends it through the pipe to the parent
		while ((n = read(f, buffer, BUF_LENGTH)) != 0) {
			if (write(fd[WRITE], buffer, n) != n) {
				fprintf(stderr, "Write error to pipe\n");
				exit(4);
			}
		}

		close(fd[WRITE]); // Closes writing side
		close(f);
		exit(5);
	}
	else { // Parent
		if (waitpid(pid, &status, 0) == -1){
			fprintf(stderr, "%s.\n", strerror(errno));
			exit(5);
		}

		close(fd[WRITE]); // Closes writing side
		dup2(fd[READ], STDIN_FILENO);
		close(fd[READ]); // Closes reading side

		// Creating a child process to execute Unix sort command
		if ((pidSort = fork()) == -1) { // Error
			fprintf(stderr, "Fork failed!\n");
			exit(2);
		}

		else if (pidSort == 0) { // Child
			// Execution of sort command
			if (execlp("sort", "sort", "-o", fileName, NULL) == -1){
				fprintf(stderr, "execlp error: %s", strerror(errno));
				exit(7);
			}
		}

		else { // Parent
			if (waitpid(pidSort, &status, 0) == -1){
				fprintf(stderr, "%s.\n", strerror(errno));
				exit(5);
			}
		}
	}

	return 0;
}

int check_dupfiles(const char* filePath){

	// TODO falta terminar

	char line[256];
	FILE* f;
	//struct fileInfo tempArr[1024];
	if ((f = fopen(filePath, "r")) == NULL){
		fprintf(stderr, "Couldn't open %s.", filePath);
		fclose(f);
		exit(1);
	}

	int ch, nFiles = 0;

	do{
		ch = fgetc(f);
		if(ch == '\n')
			nFiles++;
	} while (ch != EOF);
	fclose(f);

	if ((f = fopen(filePath, "r")) == NULL){
		fprintf(stderr, "Couldn't open %s.", filePath);
		fclose(f);
		exit(1);
	}

	fileInfo* files = malloc(nFiles*sizeof(fileInfo));
	//printf("%u\n", sizeof(fileInfo*));
	int ind = 0;
	while (fgets(line, sizeof(line), f) != NULL) {
		//printf("%s", line);
		files[ind] = load_file(line);
		//printf("%s\n", files[i].name);
		ind++;
	};
	int i,j;
	for (i = 0; i < nFiles; i++) {
		if ((i-1)<nFiles) {
			int inc = -1;
			for (j = (i+1); j < nFiles; j++) {
				int eq = equals_files(&files[i], &files[j]);
				if (eq == 0) {
					printf("%s %d is equal to %s %d\n", files[i].name,i,files[j].name,j);
					inc++;
				}
				if (inc > -1) {
					i += inc;}
			}
		}
	}

	free(files);
	fclose(f);
	return 0;
}

int equals_files(fileInfo * file1, fileInfo * file2){

	FILE *f1, *f2;
	char c1, c2;

	if (strcmp(file1->name, file2->name) != 0 || strcmp(file1->permissions, file2->permissions)!=0
			|| strcmp(file1->size, file2->size) != 0 )
		return -1; // Files are different

	if ((f1 = fopen(file1->path, "r")) == NULL){
		fprintf(stderr, "Failed to open %s.", file1->path);
		fclose(f1);
		exit(1);
	}

	if ((f2 = fopen(file2->path, "r")) == NULL){
		fprintf(stderr, "Failed to open %s.", file2->path);
		fclose(f1);
		fclose(f2);
		exit(1);
	}

	do{
		c1 = getc(f1);
		c2 = getc(f2);

		if (c1 != c2)
			return -1;

	}  while ((c1 != EOF) && (c2 != EOF));

	fclose(f1);
	fclose(f2);

	if (c1 != c2)
		return -1; // Diferent contents

	return 0;
}

fileInfo load_file(char* WfileString){

	fileInfo file;
	int i = 0;
	const char space[2] = " \n";
	char *fileString = strtok(WfileString, space);

	strcpy(file.name, fileString);
	fileString  = strtok(NULL, space);
	while( fileString  != NULL )
	{
		if (i == 0)
			strcpy(file.path, fileString);
		else if (i == 1)
			strcpy(file.date, fileString);
		else if (i == 2)
			strcpy(file.size, fileString);
		else if (i == 3)
			strcpy(file.permissions, fileString);

		fileString  = strtok(NULL, space);
		i++;
	}

	return file;
}
