CC = gcc
CFLAGS = -Wall -pthread -g

all: test_barrera

test_barrera: main.o barrier.o
	$(CC) $(CFLAGS) -o test_barrera main.o barrier.o

main.o: main.c barrier.h
	$(CC) $(CFLAGS) -c main.c

barrier.o: barrier.c barrier.h
	$(CC) $(CFLAGS) -c barrier.c

clean:
	rm -f *.o test_barrera