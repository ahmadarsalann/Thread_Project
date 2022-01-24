C = gcc

CFLAGS = -g -Wall -pthread

default: chat_room

chat_room: main.o msgq.o
	$(CC) $(CFLAGS) -o chat_room main.o msgq.o

msgq.o: msgq.c msgq.h
	$(CC) $(CFLAGS) -c msgq.c

clean:
	rm -f procprog *.o
