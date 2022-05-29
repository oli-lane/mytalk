# Makefile for asign 5

CC = gcc
CFLAGS = -Wall -g

all: mytalk

mytalk: compile
	$(CC) $(CFLAGS) -L ~pn-cs357/Given/Talk/lib64 -o mytalk mytalk.o server.o client.o -ltalk -lncurses

compile: mytalk.c server.c client.c
	$(CC) $(CFLAGS) -c -I ~pn-cs357/Given/Talk/include mytalk.c server.c client.c 

clean:
	rm -f *.o
