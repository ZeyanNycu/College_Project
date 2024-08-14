#include <iostream>
#include <stdlib.h>
#include "graph.h"
#include <sys/time.h>
#include <vector>
#include <float.h>
#include <omp.h>
using namespace std;
int minKey(double key[], bool mstSet[],int V)
{
    double min = DBL_MAX, min_index;
 
    for (int v = 0; v < V; v++)
        if (mstSet[v] == false && key[v] < min )
            min = key[v], min_index = v;
 
    return min_index;
}

Graph PrimSequential(const Graph &graph)
{
    int V = graph.num_vertices;
    cout << V << endl;
    double key[V];
    bool mstSet[V];
    vector<Edge> mst;
    mst.resize(V-1);
    for (int i = 0; i < V; i++)
        key[i] = DBL_MAX, mstSet[i] = false;
    //Transform to other status
    // First node is always root of MST
    mstSet[0] = true;
    key[0] = 0;
    int pre;
    for (int count = 0; count < V ; count++) {
        int u = minKey(key, mstSet,V);
        mstSet[u] = true;
        if(count != 0){
            mst[count-1].from = pre;
            mst[count-1].to = u;
            mst[count-1].weight = key[u];
        }
        for(int i = 0; i<graph.edges.size() ; i++){
            if(graph.edges[i].from == u){
                if(mstSet[graph.edges[i].to] == false && graph.edges[i].weight<key[graph.edges[i].to])
                {
                    key[graph.edges[i].to] = graph.edges[i].weight;
                }
            }
        }
        pre = u;
    }

    return {V,mst};
}
Graph PrimOpenMP(const Graph &graph)
{
    int V = graph.num_vertices;
    cout << V << endl;
    double key[V];
    bool mstSet[V];
    vector<Edge> mst;
    mst.resize(V-1);
    for (int i = 0; i < V; i++)
        key[i] = DBL_MAX, mstSet[i] = false;
    //Transform to other status
    // First node is always root of MST
    mstSet[0] = true;
    key[0] = 0;
    int pre;
    for (int count = 0; count < V ; count++) {
        int u = minKey(key, mstSet,V);
        mstSet[u] = true;
        pre = u;
        if(count != 0){
            mst[count-1].from = pre;
            mst[count-1].to = u;
            mst[count-1].weight = key[u];
        }
        #pragma omp parallel for 
        for(int i = 0; i<graph.edges.size() ; i++){
            int thread_id = omp_get_num_threads();
            if(graph.edges[i].from == u){
                if(mstSet[graph.edges[i].to] == false && graph.edges[i].weight<key[graph.edges[i].to])
                {
                    key[graph.edges[i].to] = graph.edges[i].weight;
                }
            }
            cout << "hello " << thread_id <<  endl;
        }
        
    }

    return {V,mst};
}
int main(int argc, char *argv[]){
    // Step 0 Read the input data
    printf("Start to read a graph %s\n",argv[1]);
    string filename = argv[1];
    string output_filename = argv[2];
    Graph graph;
    double duration;
    struct timeval start, end;
    gettimeofday(&start, nullptr);
    graph.loadGraph(filename);
    printf("Start to do prim algorithm\n");
    //Graph mst = PrimSequential(graph);
    Graph mst = PrimOpenMP(graph);
    gettimeofday(&end, nullptr);
    duration = (double) ((end.tv_sec - start.tv_sec) * 1000000u +
                         end.tv_usec - start.tv_usec) / 1.e6;
    std::cout << "PrimSequential time: " << duration << " s." << std::endl;
    std::cout << "Saving MST to " << output_filename << "." << std::endl;
    mst.saveGraph(output_filename);
    //mst.printGraph();
    return 0;
}