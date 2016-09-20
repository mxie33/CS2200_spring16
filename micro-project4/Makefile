# CS 2200 Spring 2016 - Micro Project 4 - Makefile

CC=gcc
CFLAGS= -g -Wall

all: client server

client:
	$(CC) -o client client.c $(CFLAGS)

server:
	$(CC) -o server server.c $(CFLAGS)

clean:
	rm -f server client