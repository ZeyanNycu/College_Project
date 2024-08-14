#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "boruvka_sequential.h"
#include "boruvka_parallel.h"
#include "graph.h"
#include "union_find.h"
#include "create_graph.h"
#include <sys/time.h>
#include "CycleTimer.h"

int main(int argc, char** argv){
    char* filename = argv[1];
    Graph graph = create_graph(filename);
    int n = graph->num_nodes;
    struct Edge* mst_seq;
    struct Edge* mst_par;    

    //sequential
    double startTimeSeq = CycleTimer::currentSeconds();
    mst_seq = find_MST(graph); 
    double endTimeSeq = CycleTimer::currentSeconds();
    printf("Total time sequential: %.20f\n", endTimeSeq - startTimeSeq);
    

    for(int i = 0; i < n; i++){
        if(mst_seq[i].src == 0 && mst_seq[i].dest == 0)
            continue;
        if(mst_seq[i].src < 0 || mst_seq[i].src > n || mst_seq[i].dest < 0 || mst_seq[i].dest > n)
            continue;
        printf("%d,%d\n", mst_seq[i].src, mst_seq[i].dest);
    }
    printf("end sequential\n");

    //parallelization
    double startTimePar = CycleTimer::currentSeconds();
    mst_par = find_MST_parallel(graph);
    double endTimePar = CycleTimer::currentSeconds();
    printf("Total time parallel: %.20f\n", endTimePar - startTimePar);

    for(int i = 0; i < n; i++){
        if(mst_par[i].src == 0 && mst_par[i].dest == 0)
            continue;
        if(mst_par[i].src < 0 || mst_par[i].src > n || mst_par[i].dest < 0 || mst_par[i].dest > n)
            continue;
        printf("%d,%d\n", mst_par[i].src, mst_par[i].dest);
    }

    return 0;
}
