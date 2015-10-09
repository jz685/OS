# Makefile for CSci 4061 Assignment 3.

all: test

#Program target depends on compiled Rectange and Main files 
test: test.o mini_filesystem.o
	gcc -o test  mini_filesystem.o test.o

# This section includes rules to compile .cpp files into .o files
mini_filesystem.o: mini_filesystem.h
	gcc -c mini_filesystem.c
	
test.o: mini_filesystem.h
	gcc -c test.c
	
# A target to remove all generated files.
clean:
	rm -f *.o
	rm -f mini_filesystem.o test test.o

