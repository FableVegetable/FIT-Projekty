CC=gcc

CFLAGS=-O2 -std=c11 -Wall -pedantic -lm

DEPS=socket.h

default: ipk-client.c ipk-server.c socket.o socket.h
	$(CC) $(CFLAGS) -o ipk-client socket.o ipk-client.c
	$(CC) $(CFLAGS) -o ipk-server socket.o ipk-server.c