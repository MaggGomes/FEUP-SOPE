CC = gcc

CFLAGS = -Wall

TARGET = parque clean

all: bin/parque bin/gerador

bin/parque: utilities.h parque.c
					$(CC) parque.c -pthread -lrt -o bin/parque $(CFLAGS)

bin/gerador: utilities.h gerador.c
					$(CC) gerador.c -pthread -lrt -o bin/gerador $(CFLAGS)

clean:
	$(RM) bin/*
