CC=g++
CFLAGS= -pthread

p_test.o: disjoint_set.o graph.o pthread_test.o
	$(CC) $(CFLAGS) -o p_test.o pthread_test.o disjoint_set.o graph.o 

disjoint_set.o: disjoint_set.cpp disjoint_set.h 
	$(CC)  -c disjoint_set.cpp 

graph.o: graph.cpp graph.h
	$(CC)  -c graph.cpp 

pthread_test.o: pthread_test.cpp
	$(CC) $(CFLAGS) -c pthread_test.cpp  
