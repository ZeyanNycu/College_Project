#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

#include "boruvka_parallel.h"
#include "union_find.h"

#include "CycleTimer.h"
#define THREADS 8
#define CHUNKSIZE 128
struct Edge* find_MST_parallel(Graph g){
    omp_set_num_threads(THREADS);
    int n = get_num_nodes(g);
    struct Edge* min_edges = new struct Edge[n];
    struct set *components = new struct set[n];
    int num_components = n;
    int iterations = 0;


    struct Edge* mst_edges = new struct Edge[n-1];
    int mst_edges_idx = 0;
    bool is_first_passes[n];
    int prev_num_components = 0;
    #pragma omp parallel for schedule(static)
    for(int i = 0; i < n; i++){
        components[i].parent = i;
        components[i].rank = 0;
        is_first_passes[i] = true;
    }

    double startTimeFind, endTimeFind;
    double findTotal = 0.0;
    double startTimeContract, endTimeContract;
    double contractTotal = 0.0;
    while(num_components > 1 && prev_num_components != num_components){
        prev_num_components = num_components;
        startTimeFind = CycleTimer::currentSeconds();
        #pragma omp parallel for schedule(dynamic, CHUNKSIZE)
        for(int j = 0; j < n; j++){
            if(find_parallel(components, j) == j){
            //find minimum weight edge out of each componenet
                for(int i = 0; i < n; i++){
                    int set1 = find_parallel(components, i);
                    if(set1 == j){
                        const Vertex* start = edges_begin(g, i);
                        const Vertex* end = edges_end(g, i);
                        int weight_offset = -1;
                        for(const Vertex* v = start; v < end; v++){
                            weight_offset++;
                            int set2 = find_parallel(components, *v);
                            if(set1 != set2){
                                Edge e;
                                e.src = i;
                                e.dest = *v;
                                e.weight = g->weights[g->offsets[i] + weight_offset];
                                if(is_first_passes[set1]){
                                    //weights[set1] = w;
                                    min_edges[set1] = e; 
                                    is_first_passes[set1] = false;
                                }
                                else if (min_edges[set1].weight > e.weight)
                                    min_edges[set1] = e;
                            }
                        }
                    }
                }
            }
        }
        endTimeFind = CycleTimer::currentSeconds();
        findTotal += (endTimeFind - startTimeFind);
        startTimeContract = CycleTimer::currentSeconds();
        for(int i = 0; i < n; i++){
            int src = min_edges[i].src;
            int dest = min_edges[i].dest;

            int root1 = find_seq(components, src);
            int root2 = find_seq(components, dest);
            is_first_passes[i] = true;
            if(root1 == root2){
                continue;
            }
            union_seq(components, root1, root2);
            mst_edges[mst_edges_idx] = min_edges[i];
            mst_edges_idx += 1;
            num_components--;
        }

        endTimeContract = CycleTimer::currentSeconds();
        contractTotal += (endTimeContract - startTimeContract);
        iterations++;
    }
    printf("iterations: %d\n", iterations);
    printf("find time parallel comp edge: %.20f\n", findTotal);
    printf("contract time parallel comp edge: %.20f\n", contractTotal);
    delete[] min_edges;
    delete[] components;
    return mst_edges;
}
