#include "temporalGraph.h"
#include <iostream>

int main(int argc, char* argv[]) {
    
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    const std::string filename = argv[1];

    TemporalGraph G;
    read_temporal_graph(filename, &G, true);

    std::cout << "Number of nodes: " << G.num_nodes << std::endl;
    std::cout << "Number of edges: " << G.num_edges << std::endl;
    std::cout << "Max timestep: " << G.max_timestep << std::endl;

    compute_scores(&G);

    free_temporal_graph(&G);

    std::cout << "Graph processing completed." << std::endl;

    return 0;
}