CC = gcc

CFLAGS = -Wall

TARGET = rmdup

all: bin/rmdup bin/lsdir

bin/rmdup: rmdup.h rmdup.c
	$(CC) $(CFLAGS) -o bin/rmdup rmdup.c

bin/lsdir: lsdir.h lsdir.c
	$(CC) $(CFLAGS) -o bin/lsdir lsdir.c

clean:
	$(RM) bin/*
