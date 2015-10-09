all: image_manager

CC = gcc
CFLAGS = -Wall -O2 -g -pthread

image_manager: image_manager.o
	$(CC) -pthread -o image_manager image_manager.o

image_manager.o: image_manager.c restart.h
	$(CC) $(CFLAGS) -c image_manager.c

clean:
	rm -f *.o
	rm -f image_manager
