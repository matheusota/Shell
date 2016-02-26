
CC=gcc
CFLAGS=-std=gnu99 -Wall
LIBS=-lreadline

all: shell

shell: shell.c
	$(CC) $(CFLAGS) $(LIBS) shell.c -o shell

debug: shell.c
	$(CC) $(CFLAGS) $(LIBS) -g shell.c -o shell_debug


