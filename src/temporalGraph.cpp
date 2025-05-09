#include <temporalGraph.h>


void build_temporal_graph(TemporalGraph* G, const std::vector<TemporalEdge>& edges) {
    
    G->max_timestep = 0;
    G->adj.resize(G->num_nodes);
    
    for (const auto& edge : edges) {

        G->max_timestep = std::max(G->max_timestep, edge.time);

        auto& neighbours = G->adj[edge.source];
        auto it = std::lower_bound(neighbours.begin(), neighbours.end(), edge.time,
            [](const AppearanceNeighbourhood& a, long long int t) { return a.timestep < t; });
        
        if (it == neighbours.end() || it->timestep != edge.time) {
            neighbours.insert(it, {edge.time, {edge.target}});
        } else {
            it->neighbours.push_back(edge.target);
        }

    }

}


void read_temporal_graph(const std::string& filename, TemporalGraph* G, bool directed) {

    std::ifstream infile(filename);
    std::string line;
    std::unordered_map<std::string, int> node_map;
    std::vector<TemporalEdge> edges;
    int node_count = 0;

    while(std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string source, target;
        long long int time;
        if (!(iss >> source >> target >> time)) { continue; } 

        if (node_map.count(source) == 0) {
            node_map[source] = node_count++;
        }
        if (node_map.count(target) == 0) {
            node_map[target] = node_count++;
        }

        int source_id = node_map[source];
        int target_id = node_map[target];
        if (source_id == target_id) {
            continue; 
        }

        edges.push_back({source_id, target_id, time});
        if (!directed) {
            edges.push_back({target_id, source_id, time});
        }

    }

    std::sort(edges.begin(), edges.end(), edge_cmp);
    
    G->num_nodes = node_count;
    G->num_edges = edges.size();
    build_temporal_graph(G, edges);

}

void compute_scores(TemporalGraph* G) {
    // Placeholder for score computation logic
    // This function should be implemented based on the specific scoring algorithm required
    std::cout << "Computing scores..." << std::endl;
}

void free_temporal_graph(TemporalGraph* G) {
    G->adj.clear();
    G->num_nodes = 0;
    G->num_edges = 0;
    G->max_timestep = 0;
}