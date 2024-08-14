#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
using namespace std;
#include "graph.h"

#define GRAPH_HEADER_TOKEN ((int) 0xDEADBEEF)

Graph load_graph_binary(const char* filename){
    Graph graph = (Graph)(malloc(sizeof(struct graph)));

    FILE* input = fopen(filename, "rb");

    if(!input){
        fprintf(stderr, "Could not open: %s\n", filename);
        exit(1);
    }
    int header[2];

    if(fread(header, sizeof(int), 2, input) != 2) {
        fprintf(stderr, "Error reading header.\n");
        exit(1);
    }
    //cout<<"10"<<header[0]<<header[1]<<"  "<<header[2]<<endl;
    /*if(header[0] != GRAPH_HEADER_TOKEN){
        fprintf(stderr, "Invalid graph file header. File may be corrupt.\n");
        exit(1);
    }*/
    graph->num_nodes = header[0];
    graph->num_edges = header[1];
    printf("%d\n", graph->num_edges);
    printf("%d\n", graph->num_nodes);
    graph->offsets = (int*)malloc(sizeof(int) * graph->num_nodes);
    graph->edges = (int*)malloc(sizeof(int) * graph->num_edges);
    graph->weights = (double*)malloc(sizeof(double) * graph->num_edges);
    
    if (fread(graph->offsets, sizeof(int), graph->num_nodes, input) != (size_t) graph->num_nodes) {
        fprintf(stderr, "Error reading nodes.\n");
        exit(1);
    }


    if (fread(graph->edges, sizeof(int), graph->num_edges, input) != (size_t) graph->num_edges) {
        fprintf(stderr, "Error reading edges.\n");
        exit(1);
    }

    for(int i = 0; i < graph->num_edges; i++){
        graph->weights[i] = 1;
    }


    int V = graph->num_nodes;
    int E = graph->num_edges;
    for(int i = 0; i < V; i++){
        if(i == V-1){
            for(int k = graph->offsets[i]; k < 2 * E; k++){
                printf("src %d to dest %d\n", i, graph->edges[k]);
            }
            continue;
        }
        for(int j = graph->offsets[i]; j < graph->offsets[i+1]; j++){
            printf("src %d to dest %d\n", i, graph->edges[j]);
        }
    }
    fclose(input);

    return graph;

}

void free_graph(Graph g){
    free(g->offsets);
    free(g->edges);
    free(g->weights);
    free(g);
}
