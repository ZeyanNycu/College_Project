CC=g++
CFLAGS= -pthread

pthread.o: disjoint_set.o graph.o kruskal_par_sort_pthread.o
	$(CC) $(CFLAGS) -o pthread.o kruskal_par_sort_pthread.o disjoint_set.o graph.o 

disjoint_set.o: disjoint_set.cpp disjoint_set.h 
	$(CC)  -c disjoint_set.cpp 

graph.o: graph.cpp graph.h
	$(CC)  -c graph.cpp 

kruskal_par_sort_pthread.o: kruskal_par_sort_pthread.cpp
	$(CC) $(CFLAGS) -c kruskal_par_sort_pthread.cpp  
