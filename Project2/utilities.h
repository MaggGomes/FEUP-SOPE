#ifndef UTILITIES_H
#define UTILITIES_H

#define NUM_CONTROLLERS 4
#define BUF_LENGTH 50

// Status messages
const char* ENTRY_PARK = "entrada";
const char* EXIT_PARK = "saida";
const char* FULL = "cheio!";
const char* LOG_FULL = "cheio";
const char* CLOSED = "encerrado";

// Controller's FIFO
char* fifoControllers[NUM_CONTROLLERS] = {"/tmp/fifoN", "/tmp/fifoS", "/tmp/fifoE", "/tmp/fifoO"};

typedef struct {
  clock_t parked_time;
  int v_id;
  char fifoName[BUF_LENGTH];
} info_t;

typedef struct {
  char fifoName[BUF_LENGTH];
  int stopVehicle; // 1 if is a stop vehicle,; 0 otherwise
  info_t inf;
} vehicle_t;




#endif // UTILITIES_H
