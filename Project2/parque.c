#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>

#define NUM_CONTROLLERS 4
#define NORTH 1
#define SOUTH 2
#define EAST 3
#define WEST 4

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

typedef struct {
	clock_t parkingTime;
	char entryName;
	int idParkingPlace;
	int id;
} vehiculeInfo;

// TODO - IMPLEMENTAR ESTAS FUNÇÕES QUE VAI SER USADAS PARA A EXECUÇÃO DOS THREADS

// TODO - FAZER
void* controller(void* arg);

// TODO - FAZER
void* carSaver(void* arg);

int main (int argc, char * argv[]){

	pthread_t controllerThreads[NUM_CONTROLLERS];
	int i;

  if (argc != 3 ){
		fprintf(stderr, "Usage: %s <N_LUGARES> <T_ABERTURA>\n", argv[0]);
		exit(1);
	}

	// TODO - LER ARGUMENTOS PASSADOS

	// TODO - ABRIR O FICHEIRO LOGGER


startT = clock();

	for (i = 0; i  < NUM_CONTROLLERS; i++){
		pthread_create(&controllerThreads[i], NULL, controller, fifoControllers[i]);
	}

			sleep(openTime);


	// Waits for the thread of each park's controller
	for (i = 0; i < NUM_CONTROLLERS; i++){
		pthread_join(controllerThreads[i], NULL);
	}

  pthread_exit(0);
}

// TODO - FAZER
void* controller(void* arg){




pthread_exit(NULL);
}

// TODO - FAZER
void* carSaver(void* arg){
	pthread_t selfThread = pthread_self();

	if (pthread_detach(selfThread) != 0){
		perror("Failed to make a thread detached.\n");
	exit(1);
	}

pthread_exit(NULL);
}
