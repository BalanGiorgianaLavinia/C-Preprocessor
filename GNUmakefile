CC = gcc
CFLAGS = -Wall -g -std=c89 -Wextra -D_CRT_SECURE_NO_WARNINGS

build: so-cpp

so-cpp: tema1.o utils.o
	$(CC) $^ -o $@

tema1.o: tema1.c tema1.h

utils.o: utils.c utils.h

.PHONY: clean
 
clean:
	rm -f *.o *~ so-cpp
