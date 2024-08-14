CC=g++
CFLAGS= -fopenmp

omp.o: disjoint_set.o graph.o kruskal_par_sort.o
	$(CC) $(CFLAGS) -o omp.o  kruskal_par_sort.o  disjoint_set.o graph.o 

disjoint_set.o: disjoint_set.cpp disjoint_set.h 
	$(CC)  -c  disjoint_set.cpp 

graph.o: graph.cpp graph.h
	$(CC)  -c graph.cpp 

kruskal_par_sort.o: kruskal_par_sort.cpp
	$(CC) $(CFLAGS)  -c  kruskal_par_sort.cpp  
