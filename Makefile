# ----------+ FLAGS +----------
CFLAGS = -O2 -Wall -Wextra -std=c99 -pedantic
CC = gcc

# ----------+ LINKING & COMPILING +----------
all : test

test : main.o chopdat.o dvector_ptr.o
	$(CC) $(CFLAGS) main.o chopdat.o dvector_ptr.o -o test
	etags *.[ch]

chopdat.o : chopdat.c chopdat.h
	$(CC) $(CFLAGS) -c chopdat.c -o chopdat.o

dvector_ptr.o : dvector_ptr.c dvector.h
	$(CC) $(CFLAGS) -c dvector_ptr.c -o dvector_ptr.o

main.o : main.c chopdat.h dvector.h
	$(CC) $(CFLAGS) -c main.c -o main.o

tags:
	etags *.[ch]
