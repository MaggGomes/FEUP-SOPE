CC = gcc

CFLAGS = -Wall

TARGET = parque

all: bin/parque bin/gerador

bin/parque: parque.c
					$(CC) parque.c -pthread -lrt -o bin/parque $(CFLAGS)

bin/gerador: gerador.c
					$(CC) gerador.c -pthread -lrt -o bin/gerador $(CFLAGS)

clean:
	$(RM) bin/*
