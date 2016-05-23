#ifndef UTILITIES_H
#define UTILITIES_H

#define BUF_LENGTH 50

typedef struct {
  clock_t parked_time;
  int v_id;
  char fifoName[BUF_LENGTH];
} info_t;

typedef struct {
  char fifoName[BUF_LENGTH];
  info_t inf;
} vehicle_t;




#endif // UTILITIES_H
