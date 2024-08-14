CC=g++
CFLAGS=-I. -fopenmp
DEPS = graph.h

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
prim: src/main.o src/graph.o
	$(CC) -o prim src/main.o src/graph.o 