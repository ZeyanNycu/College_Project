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

int root_pid[2];
int child_pid[2];
int child_count = 0;

typedef struct qsort_starter
{
  std::vector<Edge>::iterator begin;
  std::vector<Edge>::iterator end;
  int depth = 0;
} quickSort_parameters;

typedef struct pivot_part
{
  std::vector<Edge>::iterator iter[4];
} part;

typedef struct pthread_starter
{
  part a;
  int id = 0;
}pthread_parameters;

void* pthread_Sort(void* arg) {
    auto args = reinterpret_cast<quickSort_parameters*>(arg);
    auto begin = args->begin;
    auto end = args->end;
    int current_depth = args->depth;
    auto cmp = [](const Edge &lhs, const Edge &rhs) {
        return lhs.weight < rhs.weight;
    };

    sort(begin, end, cmp);
    return nullptr;
}

void* pthread_QuickSort(void* arg) {
    auto args = reinterpret_cast<pthread_parameters*>(arg);
    std::vector<Edge>::iterator iter[4];
    iter[0] = args->a.iter[0];
    iter[1] = args->a.iter[1];
    iter[2] = args->a.iter[2];
    iter[3] = args->a.iter[3];
    int id = args->id;
    auto cmp = [](const Edge &lhs, const Edge &rhs) {
        return lhs.weight < rhs.weight;
    };

    int pid;
    pthread_mutex_lock(&g_lock);
    if(g_activeWorkers < g_maxWorkers){
        pid = g_activeWorkers;
        std::cout << "l_pid: " << pid << std::endl;
        child_pid[id] = pid;
        g_activeWorkers++;
        pthread_mutex_unlock(&g_lock);

        quickSort_parameters depth_args_1 = {iter[0], iter[1], 1};
        pthread_create(&workers[pid], nullptr, pthread_Sort, &depth_args_1);
    }
    
    sort(iter[2], iter[3], cmp);

    pthread_join(workers[child_pid[id]], nullptr);

    return nullptr;
}

part cut(std::vector<Edge>::iterator begin, std::vector<Edge>::iterator end){
    size_t dist = std::distance(begin, end);

    auto pivot = *std::next(begin, dist / 2); // FIXME: random chosen
    part a;
    a.iter[0] = begin;
    a.iter[3] = end;

    a.iter[1] = std::partition(begin, end, [pivot](const Edge &edge) {
        return edge.weight < pivot.weight;
    });
    a.iter[2] = std::partition(a.iter[1], end, [pivot](const Edge &edge) {
        return pivot.weight >= edge.weight;
    });
    return a;
}

void* parallelQuickSort(void* arg) {
    auto args = reinterpret_cast<quickSort_parameters*>(arg);
    auto begin = args->begin;
    auto end = args->end;
    int current_depth = args->depth;

    // std::cout << "hi" << std::endl;
    // std::cout << g_activeWorkers << std::endl;
    auto cmp = [](const Edge &lhs, const Edge &rhs) {
        return lhs.weight < rhs.weight;
    };
    size_t dist = std::distance(begin, end);

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


    
    for (int i = 0; i < 2; i++){
        // std::cout << "current_depth" <<current_depth << std::endl;
        pthread_mutex_lock(&g_lock);
        if(g_activeWorkers < g_maxWorkers){
            int pid = g_activeWorkers;
            root_pid[i] = pid;
            std::cout << "root_pid: " << pid << std::endl;
            g_activeWorkers++;
            pthread_mutex_unlock(&g_lock);
            // quickSort_parameters depth_args_2 = {iter[2 * i], iter[2 * i + 1], i};
            
            part ans = cut(iter[2 * i], iter[2 * i + 1]);
            pthread_parameters depth_args_2 = {ans, i};
            
            pthread_create(&workers[pid], nullptr, pthread_QuickSort, &depth_args_2);
        }
    }

    for(int i = 0; i < 2; i++){
        pthread_join(workers[root_pid[i]], nullptr);
    }
    
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
    std::string output_filename = filename + "." + sort_algo + ".pthread_test" + ".output";
    std::cout << "Saving MST to " << output_filename << "." << std::endl;
    mst.saveGraph(output_filename);

    return 0;
}