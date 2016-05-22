#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>

/*
O programa Gerador, de forma pseudo-aleatória, “cria” viaturas e associa um novo thread a cada uma.
Cada thread associado a uma viatura (thread “viatura”) acompanha-la-á no seu “ciclo de vida”, desde que é
“criada”, passando pelo accesso e eventual estacionamento no Parque, até que sai do Parque se nele tiver
estacionado e “desaparece”.
*/

#define LOG_FILE "gerador.log"

typedef struct {
    clock_t parked_time;
    int v_id;
    char fifo_name[50];
} information_t;

typedef struct {
    char fifo_name[50];
    information_t info;
} vehicle_t;

//global variables
static clock_t initial_time;
static int t_ger;
static clock_t u_rel;
static int vID = 0;

//functions declarations

void* create_vehicle_tracker(void* arg);

char* access_point();
int parked_time();
clock_t time_between_generations();
void wait_new_vehicle();

vehicle_t create_vehicle();

//main

int main (int argc, char * argv[]){

  if (argc != 3 ){
    fprintf(stderr, "Usage: %s <T_GERACAO> <U_RELOGIO>\n", argv[0]);
    exit(1);
  }

  t_ger = atoi(argv[1]);
  u_rel = (int *) atoi(argv[2]);

  srand(time(NULL));

  pthread_t vehicle_tracker;

  initial_time = clock();
  double elapsed_time = 0;
  while (elapsed_time <= (double) t_ger){
    /*vehicle_t as = create_vehicle();
    printf("%s %s %d %Lf\n", as.fifo_name, as.info.fifo_name, as.info.v_id, (long double) as.info.parked_time);*/

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

  if (pthread_detach(selfThread) != 0){
    perror("Failed to make a thread detached.\n");
    exit(1);
  }


  pthread_exit(NULL);
}

char* access_point(){
  char* ret;

  int rd = rand()%100;

  if(rd < 25)
  ret = "/temp/fifoN";
  else if(rd < 50)
  ret = "/temp/fifoS";
  else if(rd < 75)
  ret = "/temp/fifoO";
  else if(rd < 100)
  ret = "/temp/fifoE";

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

  strcpy(ret.fifo_name, access_point());
  ret.info.v_id = vID;
  ret.info.parked_time =  parked_time();
  sprintf(ret.info.fifo_name, "%s_%d", "/temp/fifo_vh", vID);

  vID++;

  return ret;
}

FILE* create_log_file() {
    FILE* log_file;

    if ( (log_file = fopen(name, "w")) == NULL )
        return NULL;

    fprintf(log_file, "t(ticks) ; id_viat ; destin ; t_estacion ; t_vida ; observ\n");

    return log_file;
}
