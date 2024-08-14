# Borůvka's Algorithm Implementation Strategy
cd /data, and run ./data_generator <type:[r|s|p]> <param...> to generate graph

cd /graph_to_txt, and modify the first line in main.sh to choose the graph you want, and run ./main.sh to generate graph.txt

cd /src,and Run make to get the up to date executable of boruvka's algorithm

Run ./boruvka GRAPH\_FILE.txt > results.txt to output the results of a particular graph

Run "python3 validate.py results.txt" to check if the parallel and sequential version match up

