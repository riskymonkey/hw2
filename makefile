SHELL = /bin/bash
CC = gcc
SRC = $(wildcard *.c)
EXE = $(patsubst %.c, %, $(SRC))

all: ${EXE}

%:	%.c
	${CC} $@.c -o $@

clean:
	rm ${EXE}

