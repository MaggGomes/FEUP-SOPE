CC = gcc

CFLAGS = -Wall

TARGET = parque

all: bin/parque bin/gerador

bin/parque: parque.c
	$(CC) $(CFLAGS) -o bin/parque parque.c

bin/gerador: gerador.c
	$(CC) $(CFLAGS) -o bin/gerador gerador.c

clean:
	$(RM) bin/*
