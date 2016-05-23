#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include "utilities.h"

#define NUM_CONTROLLERS 4

// Global variables
const char* LOG_FILENAME = "parque.log";
const char* LOG_FULL = "cheio";
const char* LOG_CLOSED = "encerrado";
const char* LOG_PARKING = "estacionamento";
const char* LOG_EXIT= "saida";
char* fifoControllers[NUM_CONTROLLERS] = {"fifoN", "fifoS", "fifoE", "fifoO"};

static int numPlaces;
static int openTime;
static int occupiedPlaces = 0;
static clock_t startT;
static pthread_mutex_t mutexPark = PTHREAD_MUTEX_INITIALIZER;

/**
* @brief Receives the vehicles
*
* @param arg FIFO used for comunnication
*/
void* controller(void* arg);

/**
* @brief Parks the car if possible
*
* @param arg information of the car
*/
void* car_park(void* arg);

/**
* @brief Creates and opens a fifo
*
* @param pathName where the fifo will be created
* @param flag used to open the fifo
* @return the file descritor of the fifo if no error has ocurred; -1 otherwise
*/
int create_fifo(char* pathName, int flag);

/**
* Informs all the controllers to stop receiving cars
*/
void close_park();

int main (int argc, char * argv[]){

	pthread_t controllerThreads[NUM_CONTROLLERS];
	int i;

	if (argc != 3 ){
		fprintf(stderr, "Usage: %s <N_LUGARES> <T_ABERTURA>\n", argv[0]);
		exit(1);
	}

	errno = 0;
	numPlaces = strtol(argv[1], NULL, 10);
	if (errno == ERANGE || errno == EINVAL) {
		perror("convert N_LUGARES failed");
		exit(3);
	}

	errno = 0;
	openTime = strtol(argv[2], NULL, 10);
	if (errno == ERANGE || errno == EINVAL) {
		perror("convert T_ABERTURA failed");
		exit(3);
	}

	// TODO - ABRIR O FICHEIRO LOGGER

	startT = clock();

	for (i = 0; i  < NUM_CONTROLLERS; i++){
		pthread_create(&controllerThreads[i], NULL, controller, fifoControllers[i]);
	}

	sleep(10); // Park is open

	// Closes park
	close_park();

	// Waits for the thread of each park's controller
	for (i = 0; i < NUM_CONTROLLERS; i++){
		pthread_join(controllerThreads[i], NULL);
	}

	// TODO - CRIAR ESTATISTICAS

	pthread_exit(0);
}

// TODO - FAZER
void* controller(void* arg){

	int fd, n;
	char* fifoName;
	pthread_t carSaverID;

	fifoName = (char *) arg;

	if ( (fd = create_fifo(fifoName, O_RDONLY)) == -1 ){
		perror(fifoName);
		pthread_exit(NULL);
	}

	// TODO - TEMP - APAGAR
	char string[200];

	while (1) {

		n = read(fd, string, 200);

		if (n > 0)
		{
			// TODO - CORRIGIR CODIGO NESTA PARTE
			printf("%s\n", string);

			if (strcmp(string, "finish") == 0)
			break;

			pthread_create(&carSaverID, NULL, car_park, string);
		}
		else if (n == -1)
		{
			perror(fifoName);
			close(fd);
			unlink(fifoName);
			return NULL;
		}
	}

	close(fd);
	unlink(fifoName);

	pthread_exit(NULL);
}

// TODO - FAZER
void* car_park(void* arg){
	pthread_t selfThread = pthread_self();

	if (pthread_detach(selfThread) != 0){
		perror("Failed to make a thread detached.\n");
		exit(1);
	}

	pthread_exit(NULL);
}

int create_fifo(char* pathName, int flag) {
	int fd;

	if (mkfifo(pathName, S_IWUSR | S_IRUSR) == -1)
	{
		perror(pathName);
		return -1;
	}

	if ( (fd = open(pathName, flag)) == -1 )
	{
		perror("Can't open FIFO.");
		if (unlink(pathName) == -1)
		{
			perror("FIFO unlink failed");
			return -1;
		}
		return -1;
	}

	return fd;
}

// TODO - CORRIGIR
void close_park(){
	int i;
	
	for (i = 0; i < NUM_CONTROLLERS; i++){
		int fd = open(fifoControllers[i], O_WRONLY  | O_NONBLOCK);
		if (fd == -1)
		printf("%s", "error");

		write(fd, "finish", sizeof("finish"));
		close(fd);
	}
}
