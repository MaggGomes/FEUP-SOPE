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

// Global Variables
static int numPlaces;
static int openTime;
static int numOccupiedPlaces = 0;
static clock_t startT;
static pthread_mutex_t mutexPark = PTHREAD_MUTEX_INITIALIZER;
static FILE* fdLog;
const char* LOG_FILENAME = "parque.log";
static sem_t* smf;

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
* @brief Informs all the controllers to stop receiving cars
*/
void close_park();

/**
* @brief Write to the log the vehicle info
*
* @param vehicle
* @param msg to be written
*/
void update_log(info_t vehicle, const char* msg);

int main(int argc, char** argv) {

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

  // Creates a semaphore
  if ((smf = create_smf("/semaphore")) == SEM_FAILED)
  {
    exit(4);
  }

  // Opens parque.log
  if ( (fdLog = fopen(LOG_FILENAME, "w")) == NULL ){
    perror(LOG_FILENAME);
    exit(5);
  }

  fprintf(fdLog, "t(ticks) ; nlug ; id_viat ; observ\n");

  startT = clock(); // Current time
  printf("%s\n", "Park is open.");

  for (i = 0; i < NUM_CONTROLLERS; i++)
  {
    pthread_create(&controllerThreads[i], NULL, controller, fifoControllers[i]);
  }

  sleep(openTime); // Park is open

  sem_wait(smf);
  close_park(); // Closes park

  // Waits for the thread of each park's controller
  for (i = 0; i < NUM_CONTROLLERS; i++)
    pthread_join(controllerThreads[i], NULL);

  sem_post(smf);

  fclose(fdLog);

  printf("%s\n", "Park is closed.");

  pthread_exit(0);
}

void* controller(void* arg){

	int fd, n;
	char* fifoName;
	info_t* vehicle;

	pthread_t carParkID;

	fifoName = (char *) arg;

	// Creates FIFO
	if ( (fd = create_fifo(fifoName, O_RDONLY)) == -1 ){
		perror(fifoName);
		pthread_exit(NULL);
	}

  printf("Controller %s open!\n", fifoName);

	while (1) {
    vehicle = (info_t *) malloc(sizeof(info_t));
    // Reads FIFO
    n = read(fd, vehicle, sizeof(* vehicle));

		// If a vehicle is received
		if (n > 0)
		{
			if (vehicle->stopVehicle == 1){ // Stop vehicle --> close controller
        printf("%s\n", "stopVehicle");
				break;
			}
			// Creates a new thread for each car to park it if possible
			else
			pthread_create(&carParkID, NULL, car_park, vehicle);
		}
	}

	// If an error has ocurred reading the FIFO
	if (n < 0)
	perror(fifoName);

	close(fd);
  printf("Controller %s closed!\n", fifoName);

	if (unlink(fifoName) == -1)
	perror(strcat("Erro unlinking fifo ", fifoName));

	pthread_exit(NULL);
}

void* car_park(void* arg) {
  int fd;
  info_t info = *(info_t *) arg;
  message_t message, exitmsg;
  pthread_t selfThread = pthread_self();

	// Makes the thread detached
	if (pthread_detach(selfThread) != 0){
		perror("Failed to make a thread detached.\n");
		return NULL;
	}

  if ((fd = open(info.carFIFO, O_WRONLY)) == -1 )
  {
    perror(strcat(info.carFIFO, " FIFO opening failed on arrumador"));
    free(arg);
    return NULL;
  }

  pthread_mutex_lock(&mutexPark );
  if (numOccupiedPlaces >= numPlaces){
    strcpy(message.msg, FULL);
    update_log(info, LOG_FULL);
    pthread_mutex_unlock(&mutexPark);
    write(fd, &message, sizeof(message));
  }
  else {
    numOccupiedPlaces++;
    strcpy(message.msg, ENTRY_PARK);
    update_log(info, PARKING);
    write(fd, &message, sizeof(message));
    pthread_mutex_unlock(&mutexPark);

    clock_t start, end;
		start = clock();
		do {
			end = clock();
		} while(end-start <= info.parked_time);
		// Parking time is over

    pthread_mutex_lock(&mutexPark);

    numOccupiedPlaces--;
    strcpy(exitmsg.msg, EXIT_PARK);
    update_log(info, EXIT_PARK);
    pthread_mutex_unlock(&mutexPark);

    write(fd, &exitmsg, sizeof(exitmsg));
  }

  free(arg);
  close(fd);
  return NULL;
}

void close_park(){
	int fd, i;
	info_t vehicle;
	vehicle.stopVehicle = 1; // Stop vehicle

	for (i = 0; i < NUM_CONTROLLERS; i++){
		if ((fd = open(fifoControllers[i], O_WRONLY)) == -1){
			perror(strcat("Error closing controller ", fifoControllers[i]));
			return;
		}

		write(fd, &vehicle, sizeof(vehicle));
		close(fd);
	}
}

void update_log(info_t vehicle, const char* msg){
  char logMsg[BUF_LOG_LENGTH];

  sprintf(logMsg, "%8ld ; %4d ; %7d ; %s\n",
  clock() - startT, numOccupiedPlaces, vehicle.v_id, msg);

  // Writes to the log
  fprintf(fdLog, logMsg, BUF_LENGTH);
}
