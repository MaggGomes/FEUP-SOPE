#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include "utilities.h"

#define LOG_FILE "gerador.log"

//global variables
static clock_t initial_time;
static int t_ger;
static clock_t u_rel;
static int vID = 0;
static sem_t* smf;
static FILE* logger;


//auxiliar functions declarations

void* create_vehicle_tracker(void* arg);
char* access_point();
int parked_time();
clock_t time_between_generations();
void wait_new_vehicle();
int startFifo(char* fifoName, int fl);
int endFifo(char* fifoName);
sem_t* startSmf(const char* name);
FILE* startLog(char* name);
int log_inf(vehicle_t vehicle, const char* status);
int log_inf2(vehicle_t vehicle, const char* status, clock_t lifespan);

vehicle_t create_vehicle();

//main

int main (int argc, char * argv[]){

  if (argc != 3 ){
    fprintf(stderr, "Usage: %s <T_GERACAO> <U_RELOGIO>\n", argv[0]);
    exit(1);
  }
  errno = 0;
  t_ger = strtol(argv[1],NULL,10);
  if (errno == ERANGE || errno == EINVAL) {
    perror("convert T_GERACAO failed");
    exit(2);
  }
  errno = 0;
  u_rel = strtol(argv[2], NULL, 10);
  if (errno == ERANGE || errno == EINVAL) {
    perror("convert U_RELOGIO failed");
    exit(3);
  }

  if ( (logger = startLog(LOG_FILE)) == NULL )
  {
    fprintf(stderr, "Error opening %s\n", LOG_FILE);
    exit(4);
  }


  if ((smf = startSmf("/semaphore")) == SEM_FAILED ){
    exit(5);
  }

  srand(time(NULL));

  pthread_t vehicle_tracker;

  initial_time = clock();
  double elapsed_time = 0;
  while (elapsed_time <= (double) t_ger){
    vehicle_t* vehicle;
    vehicle = malloc(sizeof(vehicle_t));
    *vehicle = create_vehicle();

    pthread_create(&vehicle_tracker, NULL, create_vehicle_tracker, vehicle);

    wait_new_vehicle();

    elapsed_time = (double) (clock() - initial_time) / CLOCKS_PER_SEC;
  }

  return 0;
}

//functions definitions

void* create_vehicle_tracker(void* arg) {
  pthread_t selfThread = pthread_self();
  clock_t temp_start= clock();
  message_t tempMessage;

  if (pthread_detach(selfThread) != 0){
    perror("Failed to make a thread detached.\n");
    exit(1);
  }

  vehicle_t tempVeh = *(vehicle_t *) arg;
  int fifo_vehicle,fifo_ctr;

  if ((fifo_vehicle = startFifo(tempVeh.inf.carFIFO, O_RDWR)) == -1) {
    free(arg);
    return NULL;
  }

  sem_wait(smf);

  if ( (fifo_ctr = open(tempVeh.accessFIFO, O_WRONLY | O_NONBLOCK)) == -1 ){
    log_inf(tempVeh, "encerrado");
    endFifo(tempVeh.accessFIFO);
    free(arg);
    sem_post(smf);
    return NULL;
  }

  if (write(fifo_ctr, &tempVeh.inf, sizeof(tempVeh.inf)) == -1){
    perror("Problem writing in controller");
    endFifo(tempVeh.inf.carFIFO);
    close(fifo_ctr);
    free(arg);
    sem_post(smf);
    return NULL;
  }

  close(fifo_ctr);

  sem_post(smf);

  if (read(fifo_vehicle, &tempMessage, sizeof(tempMessage)) == -1){
    perror("Error reading from controller");
    endFifo(tempVeh.inf.carFIFO);
    close(fifo_vehicle);
    free(arg);
    return NULL;
  }

  log_inf(tempVeh, tempMessage.msg);

  if (strcmp(tempMessage.msg, ENTRY_PARK) == 0){
    if (read(fifo_vehicle, &tempMessage, sizeof(tempMessage)) == -1){
      perror("Error reading from controller(second time)");
      endFifo(tempVeh.inf.carFIFO);
      close(fifo_vehicle);
      free(arg);
      return NULL;
    }

    log_inf2(tempVeh, tempMessage.msg, (clock() - temp_start)+ tempVeh.inf.parked_time);
  }

  close(fifo_vehicle);
  free(arg);
  endFifo(tempVeh.inf.carFIFO);
  return NULL;
}

char* access_point(){
  char* ret;

  int rd = rand()%100;

  if(rd < 25)
  ret = "/tmp/fifoN";
  else if(rd < 50)
  ret = "/tmp/fifoS";
  else if(rd < 75)
  ret = "/tmp/fifoO";
  else if(rd < 100)
  ret = "/tmp/fifoE";

  return ret;
}

int parked_time(){
  int ret;

  int rd = rand()%100;

  if(rd < 10)
  ret = 1;
  else if(rd < 20)
  ret = 2;
  else if(rd < 30)
  ret = 3;
  else if(rd < 40)
  ret = 4;
  else if(rd < 50)
  ret = 5;
  else if(rd < 60)
  ret = 6;
  else if(rd < 70)
  ret = 7;
  else if(rd < 80)
  ret = 8;
  else if(rd < 90)
  ret = 9;
  else if(rd < 100)
  ret = 10;

  return ret * u_rel;
}

clock_t time_between_generations(){
  int ret;

  int rd = rand()%100;

  if(rd < 50)         // probability of 0 multiple = 50%
  ret = 0;
  else if(rd < 80)    // probability of 1 multiple = 30%
  ret = 1;
  else if(rd < 100)   // probability of 2 multiple = 20%
  ret = 2;

  return ret * u_rel;
}

void wait_new_vehicle(){
  clock_t ini = clock();
  clock_t diff = clock();
  clock_t max = time_between_generations();
  while ((diff - ini) < max){
    diff = clock();
  }
}

vehicle_t create_vehicle(){
  vehicle_t ret;
  char* ap = access_point();
  strcpy(ret.accessFIFO, ap);
  ret.inf.v_id = vID;
  ret.inf.parked_time =  parked_time();
  ret.inf.stopVehicle = 0;
  sprintf(ret.inf.carFIFO, "%s_%d", "/tmp/fifo_vh", vID);

  vID++;

  return ret;
}

int startFifo(char* fifoName, int fl) {
  int ret;
  char errorMsg[200];

  if (mkfifo(fifoName, S_IWUSR | S_IRUSR) == -1)
  {
    sprintf(errorMsg, "FIFO %s not created", fifoName);
    perror(errorMsg);
    return -1;
  }

  if ( (ret = open(fifoName, fl)) == -1 )
  {
    sprintf(errorMsg, "FIFO %s not opened", fifoName);
    perror(errorMsg);
    endFifo(fifoName);
    return -1;
  }

  return ret;
}

int endFifo(char* fifoName){
  char errorMsg[200];

  if (unlink(fifoName) == -1)
  {
    sprintf(errorMsg, "fifo %s unlink failed", fifoName);
    perror(errorMsg);
    return -1;
  }

  return 0;
}

sem_t* startSmf(const char* name) {
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

FILE* startLog(char* name) {
  FILE* ret;

  if ( (ret = fopen(name, "w")) == NULL )
  return NULL;

  fprintf(ret, "t(ticks) ; id_viat ; destin ; t_estacion ; t_vida ; observ\n");

  return ret;
}

int log_inf(vehicle_t vehicle, const char* status) {
  char msg[200];

  sprintf(msg, "%8ld ; %7d ; %4c   ; %10ld ; %6s ; %s\n",
  clock() - initial_time,
  vehicle.inf.v_id,
  vehicle.accessFIFO[strlen(vehicle.accessFIFO) - 1],
  vehicle.inf.parked_time,
  "?",
  status
  );

  return fprintf(logger, "%s", msg);
}

int log_inf2(vehicle_t vehicle, const char* status, clock_t lifespan) {
  char msg[200];
  char aux_str[100];


  sprintf(aux_str, "%ld", lifespan);

  sprintf(msg, "%8ld ; %7d ; %4c   ; %10ld ; %6s ; %s\n",
  clock() - initial_time,
  vehicle.inf.v_id,
  vehicle.accessFIFO[strlen(vehicle.accessFIFO) - 1],
  vehicle.inf.parked_time,
  aux_str,
  status
  );

  return fprintf(logger, "%s", msg);
}
