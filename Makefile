# ----------+ FLAGS +----------
CFLAGS = -O2 -Wall -Wextra -std=c99 -pedantic
INCLUDE = -Ilib
CC = gcc
DVEC = lib/dvector

# ----------+ LINKING & COMPILING +----------
all : test

test : main.o chopdat.o dvector_ptr.o
	$(CC) $(CFLAGS) $(INCLUDE) main.o chopdat.o dvector_ptr.o -o test
	etags *.[ch]

chopdat.o : chopdat.c chopdat.h
	$(CC) $(CFLAGS) $(INCLUDE) -c chopdat.c -o chopdat.o

dvector_ptr.o : $(DVEC)/dvector_ptr.c $(DVEC)/dvector.h
	$(CC) $(CFLAGS) $(INCLUDE) -c $(DVEC)/dvector_ptr.c -o dvector_ptr.o

main.o : main.c chopdat.h $(DVEC)/dvector.h
	$(CC) $(CFLAGS) $(INCLUDE) -c main.c -o main.o

tags:
	etags *.[ch]
