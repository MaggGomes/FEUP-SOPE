#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

const char* filePath = "./files.txt";
const char* hlinksPath = "./hlinks.txt";

int main(int argc, char *argv[])
{
	pid_t pid = fork();
	int status;

	// TODO usar return ou exit()
	// TODO avaliar possibilidade de erro -1 na abertura do open()
	if (argc < 2 ){
		fprintf(stderr, "Error:: Invalid number of arguments. Did you forget to mention the <directory path> ? Please invoke the program as follows: rmdup <directory path>\n\n");
		return 1;
	}

	else if (argc > 2 ){
		fprintf(stderr, "Error:: Invalid number of arguments. Please invoke the program as follows: rmdup <directory path>\n\n");
		return 2;
	}

	if (pid == -1) {
		fprintf(stderr, "Error: Fork failed!\n");
		return 3;
	}

	else if (pid == 0) { // child
		// Starts a new process, executing lsdir program on it
		execlp("./lsdir", "lsdir", argv[1], filePath, NULL);

		// Line below only is executed if lsdir fails to execute
		fprintf(stderr, "Error:: Can't execute lsdir: %s.\n", strerror(errno));
	}
	else { // parent
		if (waitpid(pid, &status, 0) == -1)
			fprintf(stderr, "Error:: %s.\n", strerror(errno));
	}

	return 0;
}
