CC=g++
CFLAGS=-I. -fopenmp
DEPS = graph.h

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
to_txt: src/main.o src/graph.o
	$(CC) -o to_txt src/main.o src/graph.o 