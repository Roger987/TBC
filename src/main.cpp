#include "temporalGraph.h"
#include "scoresComputation.h"
#include "utils.h"
#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
    
    if (argc < 7) {
        std::cerr << "Usage: " << argv[0] << " -filename <filename> -num_samples <num_samples> -seed <seed>" << std::endl;
        return 1;
    }

    parameters params;
    parse_command_line(argc, argv, &params);
    
    TemporalGraph G;
    read_temporal_graph(params.filename, &G, true);

    std::cout << "Number of nodes: " << G.num_nodes << std::endl;
    std::cout << "Number of edges: " << G.num_edges << std::endl;
    std::cout << "Max timestep: " << G.max_timestep << std::endl;

    std::unordered_map<std::pair<int, int>, double, pair_hash> scores;
    std::unordered_map<std::pair<int, int>, double, pair_hash> probs;
    compute_probs_and_scores(&G, scores, probs);

    std::vector<std::pair<int, int>> keys;
    std::vector<double> weights;
    for (const auto& [pair, prob] : probs) {
        keys.push_back(pair);
        weights.push_back(prob);
    }

    std::mt19937 rng(params.seed);
    std::discrete_distribution<> dist(weights.begin(), weights.end());

    for (int i = 0; i < params.num_samples; ++i) {
        int idx = dist(rng);
        auto [s, z] = keys[idx];
        std::cout << "Sampled pair: (" << s << ", " << z << ")\n";
    }

    free_temporal_graph(&G);

    std::cout << "Graph processing completed." << std::endl;

    return 0;
}