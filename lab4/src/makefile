CC=gcc
CFLAGS=-I.

all: sequential_min_max parallel_min_max exec_seq_min_max

sequential_min_max : utils.o find_min_max.o utils.c find_min_max.c
	$(CC) -o sequential_min_max find_min_max.o utils.o sequential_min_max.c $(CFLAGS)

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h parallel_min_max.c
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

utils.o : utils.c
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.c find_min_max.c
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

exec_seq_min_max : exec_seq_min_max.c
	$(CC) -o exec_seq_min_max exec_seq_min_max.c $(CFLAGS)

process_memory: process_memory.c
	$(CC) -w -o process_memory process_memory.c

zombie: zombie.c
	$(CC) -o zombie zombie.c

sum.o: sum.c sum.h
	$(CC) -o sum.o -c sum.c $(CFLAGS)	

libsum.a: sum.o
	ar rcs libsum.a sum.o

parallel_sum: parallel_sum.c utils.o libsum.a
	$(CC) -o parallel_sum parallel_sum.c utils.o -L. -lsum -lpthread $(CFLAGS)

clean :
	rm utils.o find_min_max.o sum.o libsum.a sequential_min_max parallel_min_max exec_seq_min_max zombie process_memory parallel_sum

