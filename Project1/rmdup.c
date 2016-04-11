#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

const char* fileName = "files.txt";
const char* hlinksName = "hlinks.txt";

int main(int argc, char *argv[])
{

	// TODO usar return ou exit()
	if (argc < 2 ){
		fprintf(stderr, "Error:: Invalid number of arguments. Did you forget to mention the <directory path> ? Please invoke the program as follows: rmdup <directory path>\n\n");
		return 1;
	}

	else if (argc > 2 ){
		fprintf(stderr, "Error:: Invalid number of arguments. Please invoke the program as follows: rmdup <directory path>\n\n");
		return 2;
	}

	// TODO Usar função open ou dup2(fd, STDOUT_FILENO) para redirecionamento de stream
	FILE* file;
	file = fopen(fileName, "w+");

	pid_t pid = fork();

	if (pid == -1) {
		fprintf(stderr, "\nError: Fork failed!\n");
		return 3;
	}

	else if (pid == 0) {
		// Starts a new child process, executing lsdir program
		remove("files.txt");
		execlp("./lsdir", "lsdir", argv[1], NULL);

		// Line below only is executed if lsdir fails to execute
		fprintf(stderr, "Error:: Can't execute lsdir: %s.\n", strerror(errno));
	}
	else {
		int status;
		(void)waitpid(pid, &status, 0);
	}

	fclose(file);

	return 0;
}
