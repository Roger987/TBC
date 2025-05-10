#ifndef SCORESCOMPUTATION_H
#define SCORESCOMPUTATION_H

#include <temporalGraph.h>
#include <utils.h>
#include <unordered_set>
#include <vector>
#include <functional>
#include <iostream>

typedef struct {
    int v;
    int time;
} VertexAppearance;

inline bool operator==(const VertexAppearance& a, const VertexAppearance& b) {
    return a.v == b.v && a.time == b.time;
}

namespace std {
    template <>
    struct hash<VertexAppearance> {
        std::size_t operator()(const VertexAppearance& va) const {
            std::size_t h1 = std::hash<int>()(va.v);
            std::size_t h2 = std::hash<int>()(va.time);
            return h1 ^ (h2 << 1);
        }
    };
}

typedef struct {
    std::vector<std::vector<int>> dists;
    std::vector<std::vector<double>> sigmas;
    std::vector<std::vector<std::unordered_set<VertexAppearance>>> preds;
    std::vector<double> totalSigmas;
    std::vector<int> totalDists;
    std::vector<int> foremostTimes;
    std::vector<VertexAppearance> stack;
} ShortestBetweennessData;


void initialize_sbd(ShortestBetweennessData* sbd, const TemporalGraph* G);
void reset_sbd_for_source(ShortestBetweennessData* sbd, int s);
bool compute_sigmas_temporal(const TemporalGraph* G, bool strict, int s, int z, ShortestBetweennessData* sbd);
double compute_score(const TemporalGraph* G, int s, int z);
void compute_probs_and_scores(const TemporalGraph* G, std::unordered_map<std::pair<int, int>, double, pair_hash>& scores, std::unordered_map<std::pair<int, int>, double, pair_hash>& probs);

#endif // SCORESCOMPUTATION_H