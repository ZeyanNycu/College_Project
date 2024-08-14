LDFLAGS = -$(shell uname -m)
CFLAGS	= -Wall -g -D_GNU_SOURCE -fPIC
all:
	gcc -c sandbox.c $(CFLAGS) -o sandbox.o
	gcc -o sandbox.so -shared sandbox.o $(LDFLAGS)