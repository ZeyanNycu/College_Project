#include <iostream>
#include <stdlib.h>
#include "graph.h"
#include <sys/time.h>
#include <vector>
#include <float.h>
#include <omp.h>
using namespace std;

int main(int argc, char *argv[]){
    printf("Start to read a graph %s\n",argv[1]);
    string filename = argv[1];
    string output_filename = argv[2];
    Graph graph;
    //gettimeofday(&start, nullptr);
    graph.loadGraph(filename);
    graph.jerry(filename);
    printf("tranform graph to %s.txt\n",argv[1]);
    return 0;
}