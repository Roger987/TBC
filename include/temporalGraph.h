#ifndef TEMPORAL_GRAPH_H
#define TEMPORAL_GRAPH_H

#include <vector>
#include <set>
#include <fstream>
#include <sstream>
#include <unordered_map>

struct TemporalEdge {
    int source;
    int target;
    long long int time;
};

typedef struct {
    long long timestep;
    std::vector<int> neighbours;
} AppearanceNeighbourhood;

typedef struct {
    int num_nodes;
    long long int num_edges;
    long long max_timestep;
    std::vector<std::vector<AppearanceNeighbourhood>> adj;
} TemporalGraph;

void read_temporal_graph(const std::string& filename, TemporalGraph* G, bool directed);
void build_temporal_graph(TemporalGraph* G, const std::vector<TemporalEdge>& edges);
void compute_scores(TemporalGraph* G);
void free_temporal_graph(TemporalGraph* G);

bool edge_cmp(const TemporalEdge& a, const TemporalEdge& b) {
    return std::tie(a.time, a.source, a.target) < std::tie(b.time, b.source, b.target);
}

#endif