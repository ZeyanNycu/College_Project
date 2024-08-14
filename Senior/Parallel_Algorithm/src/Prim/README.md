# Prim algorithm
---
Here, I intend to parallelize the Prim's algorithm and modify my research content based on the comments from the TA.
## Progress
---
1. We need to implement serialize Prim algorithm  ,use its result as our baseline and finally do some analysis based on my research.
2. 
## Some suggestion from TA
---
1. What kinds of parallelism can you exploit? Can you extend the algorithm/data structure to multiple machines or heterogeneous devices (e.g., GPU)?
2. Can you mix different parallelism strategies in your algorithm/data structure?
3. Can the compiler easily achieve your manual SIMD optimization? Or can you find some vectorization opportunities that the compiler missed?
4. Is it possible that your manual multi-threading implementation is equivalent to your OpenMP implementation? What are the differences between these two implementations?
5. Besides parallelizing your serial implementation, can you also implement at least one published parallel version of the algorithm/data structure for comparison? You do not have to beat it of course.
6. What speedup can be achieved by your algorithm/data structure on what scale of the problem?