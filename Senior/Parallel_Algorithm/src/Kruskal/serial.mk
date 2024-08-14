CC=g++
# CFLAGS=-I. -fopenmp
# DEPS = graph.h

# %.o: %.cpp $(DEPS)
# 	$(CC) -c -o $@ $< $(CFLAGS)
# prim: src/main.o src/graph.o
# 	$(CC) -o prim src/main.o src/graph.o 

a.out: disjoint_set.o graph.o kruskal_seq.o
	$(CC) -o a.out disjoint_set.o graph.o kruskal_seq.o

disjoint_set.o: disjoint_set.cpp disjoint_set.h
	$(CC) -c disjoint_set.cpp

graph.o: graph.cpp graph.h
	$(CC) -c graph.cpp

kruskal_seq.o: kruskal_seq.cpp
	$(CC) -c kruskal_seq.cpp