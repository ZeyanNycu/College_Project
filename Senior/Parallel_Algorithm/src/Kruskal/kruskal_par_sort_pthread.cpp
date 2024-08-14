//
// Created by Xuren Zhou on 11/17/19.
//
#include <algorithm>
#include <iostream>
#include <vector>
#include <sys/time.h>
#include "disjoint_set.h"
#include "graph.h"
#include <pthread.h>

#define PAR_SORT_THRESHOLD 8192
#define MAX_WORKERS 4

int g_activeWorkers = 0;
int g_maxWorkers = 4;

pthread_mutex_t g_lock;
pthread_t workers[MAX_WORKERS];
pthread_attr_t g_attr;

typedef struct qsort_starter
{
  std::vector<Edge>::iterator begin;
  std::vector<Edge>::iterator end;
  int depth = 0;
} quickSort_parameters;

void* pthread_QuickSort(void* arg) {
    auto args = reinterpret_cast<quickSort_parameters*>(arg);
    auto begin = args->begin;
    auto end = args->end;
    auto cmp = [](const Edge &lhs, const Edge &rhs) {
        return lhs.weight < rhs.weight;
    };
    // size_t dist = std::distance(begin, end);
    // if (dist <= PAR_SORT_THRESHOLD) {
    //     sort(begin, end, cmp);
    //     // std::cout << "hi" << std::endl;
    //     return nullptr;
    // }
    sort(begin, end, cmp);
    return nullptr;
}

void* parallelQuickSort(void* arg) {
    auto args = reinterpret_cast<quickSort_parameters*>(arg);
    auto begin = args->begin;
    auto end = args->end;
    int current_depth = args->depth;

    
    // std::cout << g_activeWorkers << std::endl;
    auto cmp = [](const Edge &lhs, const Edge &rhs) {
        return lhs.weight < rhs.weight;
    };
    size_t dist = std::distance(begin, end);
    if (dist <= PAR_SORT_THRESHOLD) {
        sort(begin, end, cmp);
        // std::cout << "hi" << std::endl;
        return nullptr;
    }

    auto pivot = *std::next(begin, dist / 2); // FIXME: random chosen
    std::vector<Edge>::iterator iter[4];
    iter[0] = begin;
    iter[3] = end;

    iter[1] = std::partition(begin, end, [pivot](const Edge &edge) {
        return edge.weight < pivot.weight;
    });
    iter[2] = std::partition(iter[1], end, [pivot](const Edge &edge) {
        return pivot.weight >= edge.weight;
    });


    if(current_depth == 1){
        pthread_mutex_lock(&g_lock);
        if(g_activeWorkers < g_maxWorkers){
            int pid = g_activeWorkers;
            std::cout << pid << std::endl;
            g_activeWorkers++;
            pthread_mutex_unlock(&g_lock);

            quickSort_parameters depth_args_1 = {iter[2 * 0], iter[2 * 0 + 1], current_depth+1};
            pthread_create(&workers[pid], nullptr, parallelQuickSort, &depth_args_1);
            // pthread_create(&threads[i], nullptr, parallelQuickSort, &thread_args[i]);

            pthread_join(workers[pid], nullptr);

            // quickSort_parameters depth_args_2 = {iter[2 * 1], iter[2 * 1 + 1], current_depth};
            // parallelQuickSort(&depth_args_2);
            return nullptr;
        }

        pthread_mutex_lock(&g_lock);
        if(g_activeWorkers < g_maxWorkers){
            int pid = g_activeWorkers;
            std::cout << pid << std::endl;
            g_activeWorkers++;
            pthread_mutex_unlock(&g_lock);

            quickSort_parameters depth_args_2 = {iter[2 * 1], iter[2 * 1 + 1], current_depth+1};
            pthread_create(&workers[pid], nullptr, parallelQuickSort, &depth_args_2);
            // pthread_create(&threads[i], nullptr, parallelQuickSort, &thread_args[i]);

            pthread_join(workers[pid], nullptr);

            return nullptr;
        }
    }
    else{
        for (int i = 0; i < 2; i++){
            quickSort_parameters depth_args_2 = {iter[2 * i], iter[2 * i + 1], current_depth+1};
            parallelQuickSort(&depth_args_2);
        }
    }
    

    // pthread_mutex_lock(&g_lock);
    // if(g_activeWorkers < g_maxWorkers)
    // {
    //     int pid = g_activeWorkers;
    //     std::cout << pid << std::endl;
    //     g_activeWorkers++;
    //     pthread_mutex_unlock(&g_lock);

    //     quickSort_parameters depth_args_1 = {iter[2 * 0], iter[2 * 0 + 1], current_depth};
    //     pthread_create(&workers[pid], nullptr, parallelQuickSort, &depth_args_1);
    //     // pthread_create(&threads[i], nullptr, parallelQuickSort, &thread_args[i]);

    //     pthread_join(workers[pid], nullptr);

    //     quickSort_parameters depth_args_2 = {iter[2 * 1], iter[2 * 1 + 1], current_depth};
    //     parallelQuickSort(&depth_args_2);
    //     return nullptr;
    // }
    // else{
    //     pthread_mutex_unlock(&g_lock);
    //     sort(begin, end, cmp);
    //     // std::cout << "hi" << std::endl;
    //     return nullptr;
    // }
    return nullptr;
}

Graph kruskalMSTSequential(const Graph &graph, std::string sort_algo) {
    struct timeval start, end;
    double duration;
    double total = 0;

    // Copy and sort the edges by weight.
    std::vector<Edge> edges = graph.edges;
    std::cout << "Length of edges: " << edges.size() << std::endl;
    gettimeofday(&start, nullptr);

    if (sort_algo == "QS") {
        quickSort_parameters args = {edges.begin(), edges.end(), 0};
        // std::pair<std::vector<Edge>::iterator, std::vector<Edge>::iterator> args(edges.begin(), edges.end());
        parallelQuickSort(&args);
    }

    gettimeofday(&end, nullptr);
    duration = (double) ((end.tv_sec - start.tv_sec) * 1000000u +
                         end.tv_usec - start.tv_usec) / 1.e6;
    total += duration;
    std::cout << "Sort time: " << duration << " s." << std::endl;

    // Kruskal algorithm.
    gettimeofday(&start, nullptr);
    DisjointSet ds(graph.num_vertices);
    std::vector<Edge> mst_edges;
    for (const Edge e : edges) {
        if (!ds.belongSameSet(e.from, e.to)) {
            ds.unionSet(e.from, e.to);
            mst_edges.push_back(e);
        }
    }
    gettimeofday(&end, nullptr);
    duration = (double) ((end.tv_sec - start.tv_sec) * 1000000u +
                         end.tv_usec - start.tv_usec) / 1.e6;
    std::cout << "Merging time: " << duration << " s." << std::endl;

    total += duration;
    std::cout << "total time: " << total << " s." << std::endl;
    std::cout << "sort percent: " << 1 - duration / total << std::endl;
    return {graph.num_vertices, mst_edges};
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr
                << "Invalid command: bin/kruskal_par_sort <data_path> <QS|ES|SS>"
                  << std::endl;
        return 1;
    }
    std::string filename = argv[1];
    std::string sort_algo = argv[2];
    std::cout << "File name: " << filename << " Sort algorithm: " << sort_algo
              << std::endl;
    struct timeval start, end;
    double duration;

    gettimeofday(&start, nullptr);
    Graph graph;
    graph.loadGraph(filename);
    gettimeofday(&end, nullptr);
    duration = (double) ((end.tv_sec - start.tv_sec) * 1000000u +
                         end.tv_usec - start.tv_usec) / 1.e6;
    std::cout << "loading time: " << duration << " s." << std::endl;

    pthread_attr_init(&g_attr);
	pthread_attr_setscope(&g_attr, PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setdetachstate(&g_attr, PTHREAD_CREATE_JOINABLE);

    pthread_mutex_init(&g_lock, NULL);

    // Execute the algorithm and print the MST.
    Graph mst = kruskalMSTSequential(graph, sort_algo);

    // Save the result.
    std::string output_filename = filename + "." + sort_algo + ".pthread" + ".output";
    std::cout << "Saving MST to " << output_filename << "." << std::endl;
    mst.saveGraph(output_filename);

    return 0;
}