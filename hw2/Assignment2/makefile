# Makefile for CSci 4061 Assignment 2.

all: parallel_convert

############################## Program 

#Program target depends on compiled Rectange and Main files 
parallel_convert: parallel_convert.o 
	g++ -o parallel_convert parallel_convert.o 

# This section includes rules to compile .cpp files into .o files
parallel_convert.o: restart.h
	g++ -c parallel_convert.c
	
# A target to remove all generated files.
clean:
	rm -f *.o
	rm -f parallel_convert

