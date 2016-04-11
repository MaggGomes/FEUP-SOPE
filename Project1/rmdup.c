#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

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

	FILE * file = fopen(fileName, "a+");

	pid_t pid = fork();

	if (pid == -1) {
		fprintf(stderr, "Error: Fork failed!\n");
		return 3;
	}

	else if (pid == 0) { // child
		// Starts a new process, executing lsdir program on it
		execlp("./lsdir", "lsdir", argv[1], NULL);

		// Line below only is executed if lsdir fails to execute
		fprintf(stderr, "Error:: Can't execute lsdir: %s.\n", strerror(errno));
	}
	else { // parent
		waitpid(pid, NULL, 0);
		fclose(file);
	}

	return 0;
}
