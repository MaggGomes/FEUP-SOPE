#ifndef UTILITIES_H
#define UTILITIES_H

#include <semaphore.h>

#define NUM_CONTROLLERS 4
#define BUF_LENGTH 100
#define BUF_LOG_LENGTH 512

// Status messages
const char ENTRY_PARK[] = "entrada";
const char PARKING[] = "estacionamento";
const char EXIT_PARK[] = "saida";
const char FULL[] = "cheio!";
const char LOG_FULL[] = "cheio";
const char CLOSED[] = "encerrado";

// Controller's FIFO
char* fifoControllers[NUM_CONTROLLERS] = {"/tmp/fifoN", "/tmp/fifoS", "/tmp/fifoE", "/tmp/fifoO"};

typedef struct {
  clock_t parked_time;
  int v_id;
  char carFIFO[BUF_LENGTH];
  int stopVehicle; // 1 if is a stop vehicle,; 0 otherwise
} info_t;

typedef struct {
  char accessFIFO[BUF_LENGTH];
  info_t inf;
} vehicle_t;

typedef struct {
    char msg[BUF_LENGTH];
} message_t;

/**
* @brief Creates and opens a fifo
*
* @param pathName where the fifo will be created
* @param flag used to open the fifo
* @return the file descriptor of the fifo if no error has ocurred; -1 otherwise
*/
int create_fifo(char* pathName, int flag) {
	int fd;

	if (mkfifo(pathName, S_IWUSR | S_IRUSR) == -1)
	{
		perror(pathName);
		return -1;
	}

	if ((fd = open(pathName, flag)) == -1 )
	{
		perror(pathName);
		if (unlink(pathName) == -1)
		{
			perror("FIFO unlink failed");
			return -1;
		}
		return -1;
	}

	return fd;
}

/**
* @brief Creates a semaphore
*
* @param name
* @return sempahore created
*/
sem_t* create_smf(const char* name) {
  sem_t* ret;

  if( (ret = sem_open(name, O_CREAT | O_EXCL, S_IWUSR | S_IRUSR, 1)) == SEM_FAILED ){
    if (errno == EEXIST){
      if( (ret = sem_open(name, 0)) == SEM_FAILED ){
        perror("Semaphore not opened");
        return SEM_FAILED;
      }
    }else{
      perror("Semaphore not created");
      return SEM_FAILED;
    }
  }

  return ret;
}

#endif // UTILITIES_H
