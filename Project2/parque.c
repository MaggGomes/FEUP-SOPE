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

// Global variables
static int numPlaces;
static int openTime;
static int numOccupiedPlaces = 0;
static clock_t startT;
static pthread_mutex_t mutexPark = PTHREAD_MUTEX_INITIALIZER;
static FILE* fdLog;
const char* LOG_FILENAME = "parque.log";

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
* @return the file descriptor of the fifo if no error has ocurred; -1 otherwise
*/
int create_fifo(char* pathName, int flag);

/**
* @brief Informs all the controllers to stop receiving cars
*/
void close_park();

/**
* @brief Write to the log the vehicle info
*
* @param vehicle
* @param msg to be written
*/
void update_log(vehicle_t vehicle, const char* msg);

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
		exit(2);
	}

	errno = 0;
	openTime = strtol(argv[2], NULL, 10);
	if (errno == ERANGE || errno == EINVAL) {
		perror("convert T_ABERTURA failed");
		exit(3);
	}

	if ( (fdLog = fopen(LOG_FILENAME, "w")) == NULL ){
		perror(LOG_FILENAME);
		exit(4);
	}

	fprintf(fdLog, "t(ticks) ; nlug ; id_viat ; observ\n");

	startT = clock(); // Current time

	// Creates a thread for each controller
	for (i = 0; i  < NUM_CONTROLLERS; i++){
		pthread_create(&controllerThreads[i], NULL, controller, fifoControllers[i]);
	}

	sleep(openTime); // Park is open

	close_park(); // Closes park

	// Waits for the thread of each park's controller
	for (i = 0; i < NUM_CONTROLLERS; i++){
		pthread_join(controllerThreads[i], NULL);
	}

	fclose(fdLog);

	pthread_exit(0);
}

void* controller(void* arg){

	int fd, n;
	char* fifoName;
	vehicle_t vehicle;

	pthread_t carParkID;

	fifoName = (char *) arg;

	// Creates FIFO
	if ( (fd = create_fifo(fifoName, O_RDONLY)) == -1 ){
		perror(fifoName);
		pthread_exit(NULL);
	}

	// Reads FIFO
	while ((n = read(fd, &vehicle, sizeof(vehicle_t))) >= 0) {
		// If a vehicle is received
		if (n > 0)
		{
			if (vehicle.stopVehicle == 1){ // Stop vehicle --> close controller
				break;
			}
			// Creates a new thread for each car to park it if possible
			else
			pthread_create(&carParkID, NULL, car_park, &vehicle);
		}
	}

	// If an error has ocurred reading the FIFO
	if (n < 0)
	perror(fifoName);

	close(fd);

	if (unlink(fifoName) == -1)
	perror(fifoName);

	pthread_exit(NULL);
}

// TODO - FAZER
void* car_park(void* arg){

	int fd;
	char msg[BUF_LENGTH];
	vehicle_t vehicle = *(vehicle_t *) arg;
	pthread_t selfThread = pthread_self();

	// Makes the thread detached
	if (pthread_detach(selfThread) != 0){
		perror("Failed to make a thread detached.\n");
		return NULL;
	}

	if ((fd = open(vehicle.inf.carFIFO, O_WRONLY  | O_NONBLOCK)) == -1){
		perror(vehicle.inf.carFIFO);
		return NULL;
	}

	pthread_mutex_lock(&mutexPark);

	if (numOccupiedPlaces < numPlaces){
		numOccupiedPlaces++;

	}

	else {
		write(fd, FULL, sizeof(FULL));
		update_log(vehicle, FULL);
	}


	clock_t start, end;
	start = clock();
	do {
		end = clock();
	} while(end-start <= vehicle.inf.parked_time);


	pthread_mutex_unlock(&mutexPark);



	numOccupiedPlaces--;

	/*


	// Validar entrada (Zona critica)
	pthread_mutex_lock(&arrumador_lock);

	if (numLugaresOcupados < numLugares)
	{
	accepted = 1;
	numLugaresOcupados++;
	strcpy(feedback.msg, ACCEPTED_STR);
	park_log(info, LOG_ACCEPTED_STR);
}
else {
strcpy(feedback.msg, FULL_STR);
park_log(info, LOG_FULL_STR);
}

pthread_mutex_unlock(&arrumador_lock);
/*********************************/

/*  write(fd_vehicle, &feedback, sizeof(feedback));

if (accepted)
{
feedback_t exit_feedback;

// Esperar
wait_ticks(info.parking_time);

// Validar saida (Zona critica)
pthread_mutex_lock(&arrumador_lock);

numLugaresOcupados--;
strcpy(exit_feedback.msg, EXITING_STR);
park_log(info, LOG_EXITING_STR);

pthread_mutex_unlock(&arrumador_lock);
/*********************************/

/*  write(fd_vehicle, &exit_feedback, sizeof(exit_feedback));
}


free(arg);
close(fd_vehicle);
return NULL;*/

return NULL;
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

void close_park(){
	int fd, i;
	vehicle_t vehicle;
	vehicle.stopVehicle = 1; // Stop vehicle

	for (i = 0; i < NUM_CONTROLLERS; i++){
		if ((fd = open(fifoControllers[i], O_WRONLY  | O_NONBLOCK)) == -1){
			perror(fifoControllers[i]);
			return;
		}

		write(fd, &vehicle, sizeof(vehicle_t));
		close(fd);
	}
}

void update_log(vehicle_t vehicle, const char* msg){
	char logMsg[BUF_LENGTH];
	// TODO - CORRIGIR
	sprintf(logMsg, "%8ld ; %4d ; %7d ; %s\n",
	clock() - startT, 0, vehicle.inf.v_id, "cheio");

	// Writes to the log
	fprintf(fdLog, logMsg, strlen(logMsg));
}
