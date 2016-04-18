CC = gcc

CFLAGS = -Wall

TARGET = rmdup

all: rmdup lsdir

rmdup: rmdup.h rmdup.c
	$(CC) $(CFLAGS) -o bin/rmdup rmdup.c

lsdir: lsdir.h lsdir.c
	$(CC) $(CFLAGS) -o bin/lsdir lsdir.c

clean:
	$(RM) bin/*
