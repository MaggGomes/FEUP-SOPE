#include "rmdup.h"

const char* filePath = "./files.txt";
const char* hlinksPath = "/hlinks.txt";

int main(int argc, char* argv[]) {
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
			fprintf(stderr, "execlp error: %s.\n", strerror(errno));
			exit(3);
		}

		exit(0);
	}
	else { // Parent
		if (waitpid(pid, &status, 0) == -1){
			fprintf(stderr, "%s.\n", strerror(errno));
			exit(4);
		}

		sort_file(filePath);
		check_dupfiles(filePath, argv[1]);
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
		fprintf(stderr, "Pipe error.\n");
		exit(1);
	}

	if ((pid = fork()) == -1) { // Error
		fprintf(stderr, "Fork failed!\n");
		exit(2);
	}

	else if (pid == 0) { // Child
		close(fd[READ]); // Closes reading side

		if ((f = open(fileName, O_RDONLY)) == -1){
			fprintf(stderr, "Failed to open %s(in sort_file).\n", fileName);
			exit(3);
		}

		// Reads the information of each file and sends it through the pipe to the parent
		while ((n = read(f, buffer, BUF_LENGTH)) != 0) {
			if (write(fd[WRITE], buffer, n) != n) {
				fprintf(stderr, "Write error to pipe.\n");
				exit(4);
			}
		}

		close(fd[WRITE]); // Closes writing side
		close(f);
		exit(0);
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
			exit(6);
		}

		else if (pidSort == 0) { // Child
			/* Execution of sort command
			Because the way the file information was saved (name, date, others),
			the sort will starting sorting from name and then, in case of equals names,
			by date */
			if (execlp("sort", "sort", "-o", fileName, NULL) == -1){
				fprintf(stderr, "execlp error: %s.\n", strerror(errno));
				exit(7);
			}
		}

		else { // Parent
			if (waitpid(pidSort, &status, 0) == -1){
				fprintf(stderr, "%s.\n", strerror(errno));
				exit(8);
			}
		}
	}

	return 0;
}

int check_dupfiles(const char* filePath, char* directory){

	FILE * f1, * f2;
	int i, j, numFiles;
	char line[BUF_LENGTH];
	char *hlinks = strcat(directory, hlinksPath);
	fileInfo* files = (fileInfo*)malloc(0);

	if ((f1 = fopen(filePath, "r")) == NULL){
		fprintf(stderr, "Failed to open %s (in check_dupfiles).\n", filePath);
		fclose(f1);
		exit(1);
	}

	if ((f2 = fopen(hlinks, "w")) == NULL){
		fprintf(stderr, "Failed to open %s (in check_dupfiles).\n", hlinks);
		fclose(f1);
		fclose(f2);
		exit(2);
	}

	i = numFiles =0;
	while (fgets(line, sizeof(line), f1) != NULL) {
		files= realloc(files, (i+1)*sizeof(fileInfo));
		files[i] = load_file(line);
		i++;
		numFiles++;
	};

	fclose(f1);

	// The files are sorted by name and date (less to most recent)
	for (i = 0; i < numFiles-1; i++){
		for (j = 0; j < numFiles; j++){
			if (files[i].isDupFile)
				break;
			else {
				if (equals_files(&files[i], &files[i+j+1]) == 0){
					files[i+j+1].isDupFile = 1;
					unlink(files[i+j+1].path);
					link(files[i].path, files[i+j+1].path);
					fprintf(f2, "%-55s %-20s %-55s\n", files[i+j+1].path, "---link to-->", files[i].path);
				}
			}
		}
	}

	free(files);
	fclose(f2);

	return 0;
}

int equals_files(fileInfo * file1, fileInfo * file2){

	FILE *f1, *f2;
	char c1, c2;

	if (strcmp(file1->name, file2->name) != 0 || strcmp(file1->permissions, file2->permissions)!=0
			|| strcmp(file1->size, file2->size) != 0 )
		return -1; // Files are different

	if ((f1 = fopen(file1->path, "r")) == NULL){
		fprintf(stderr, "Failed to open %s (in equals_files).\n", file1->path);
		exit(1);
	}

	if ((f2 = fopen(file2->path, "r")) == NULL){
		fprintf(stderr, "Failed to open %s (in equals_files).\n", file2->path);
		fclose(f1);
		exit(2);
	}

	do{
		c1 = getc(f1);
		c2 = getc(f2);

		if (c1 != c2)
			return -1;

	}  while (!(feof(f1) || feof(f2)) && (c1 == c2));

	fclose(f1);
	fclose(f2);

	if (c1 != c2)
		return -1; // Diferent contents

	return 0;
}

fileInfo load_file(char* fileString){

	fileInfo file;
	char *fileToken;

	fileToken = strtok(fileString, "|");
	strcpy(file.name, fileToken);

	fileToken = strtok(NULL, " ");
	strcpy(file.date, fileToken);

	fileToken = strtok(NULL, " ");
	strcpy(file.size, fileToken);

	fileToken = strtok(NULL, " ");
	strcpy(file.permissions, fileToken);

	fileToken = strtok(NULL, "\n");
	strcpy(file.path, fileToken);

	// All files are initialized as being not duplicated from another one (value 0)
	file.isDupFile = 0;

	return file;
}
