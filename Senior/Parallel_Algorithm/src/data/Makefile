CC=g++
CFLAGS=-I.
DEPS = graph.h

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
data_generator: src/main.o src/graph.o
	$(CC) -o data_generator src/main.o src/graph.o 