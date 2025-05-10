#include <scoresComputation.h>

void initialize_sbd(ShortestBetweennessData* sbd, const TemporalGraph* G) {
    int n = G->num_nodes;
    int T = G->max_timestep + 1;
    sbd->dists.assign(n, std::vector<int>(T, -1));
    sbd->sigmas.assign(n, std::vector<double>(T, 0));
    sbd->preds.assign(n, std::vector<std::unordered_set<VertexAppearance>>(T));
    sbd->totalSigmas.assign(n, 0);
    sbd->totalDists.assign(n, -1);
    sbd->foremostTimes.assign(n, -1);
}

void reset_sbd_for_source(ShortestBetweennessData* sbd, int s) {
    for (auto& d : sbd->dists) {
        std::fill(d.begin(), d.end(), -1);
    }
    for (auto& sig : sbd->sigmas) {
        std::fill(sig.begin(), sig.end(), 0);
    }
    for (auto& pred : sbd->preds) {
        for (auto& p : pred) {
            p.clear();
        }
    }
    std::fill(sbd->totalSigmas.begin(), sbd->totalSigmas.end(), 0);
    std::fill(sbd->totalDists.begin(), sbd->totalDists.end(), -1);
    std::fill(sbd->foremostTimes.begin(), sbd->foremostTimes.end(), -1);
    sbd->stack.clear();

    sbd->dists[s][0] = 0;
    sbd->sigmas[s][0] = 1;
    sbd->totalSigmas[s] = 1;
    sbd->totalDists[s] = 0;
}

bool compute_sigmas_temporal(const TemporalGraph* G, bool strict, int s, int z, ShortestBetweennessData* sbd) {
    
    std::queue<VertexAppearance> q;
    int mindist_to_z{-1};
    q.push(VertexAppearance{s, 0});

    while(!q.empty()){

        auto cur = q.front();
        q.pop();

        if ((mindist_to_z == -1) || ((sbd->dists[cur.v][cur.time] < mindist_to_z) && mindist_to_z > 0)) {
            
            auto& timeline = G->adj[cur.v];
            long long min_next_time = cur.time + (strict ? 1 : 0);
            auto it = std::lower_bound(timeline.begin(), timeline.end(),min_next_time,
                [](const AppearanceNeighbourhood& a, long long t) { return a.timestep < t; });
            for( ; it != timeline.end(); ++it) {
                int t = it->timestep;
                for (int w : it->neighbours) {
                    if (sbd->dists[w][t] < 0) {
                        sbd->dists[w][t] = sbd->dists[cur.v][cur.time] + 1;
                        if (sbd->totalDists[w] < 0) {
                            sbd->totalDists[w] = sbd->dists[w][t];
                            if (w == z) {
                                mindist_to_z = sbd->dists[w][t];
                            }
                        }
                        if (w != z) {
                            if ((mindist_to_z < 0) || ((mindist_to_z > 0) && (sbd->dists[w][t] < mindist_to_z))) {
                                q.push(VertexAppearance{w, t});
                            }
                        }
                        sbd->stack.push_back(VertexAppearance{w, t});
                        
                    }
                    if (sbd->dists[w][t] == sbd->dists[cur.v][cur.time] + 1) {
                        sbd->sigmas[w][t] += sbd->sigmas[cur.v][cur.time];
                        sbd->preds[w][t].insert(cur);
                        if (sbd->totalDists[w] == sbd->dists[cur.v][cur.time] + 1) {
                            sbd->totalSigmas[w] += sbd->sigmas[cur.v][cur.time];
                        }
                    } 
                }
            } 

        }

    }

    return mindist_to_z > 0;
} 

double compute_score(const TemporalGraph* G, int s, int z) {
    static ShortestBetweennessData sbd;
    static bool first = true;
    if (first) {
        initialize_sbd(&sbd, G);
        first = false;
    }
    reset_sbd_for_source(&sbd, s);

    bool reached_z = compute_sigmas_temporal(G, true, s, z, &sbd);
    double sigma_sz = sbd.totalSigmas[z];
    if (!reached_z || sigma_sz == 0.0) {
        return 0.0;
    }

    std::unordered_set<VertexAppearance> visited;
    std::stack<VertexAppearance> stk;
    for (int t = 0; t <= G->max_timestep; ++t) {
        if (sbd.dists[z][t] == sbd.totalDists[z]) {
            visited.insert(VertexAppearance{z, t});
            stk.push(VertexAppearance{z, t});
        }
    }
    while (!stk.empty()) {
        auto cur = stk.top(); stk.pop();
        for (auto p : sbd.preds[cur.v][cur.time]) {
            if (visited.insert(p).second) {
                stk.push(p);
            }
        }
    }

    std::unordered_map<VertexAppearance, std::vector<VertexAppearance>> succs;
    for (auto &va : visited) {
        for (auto &p : sbd.preds[va.v][va.time]) {
            succs[p].push_back(va);
        }
    }

    std::unordered_map<VertexAppearance, double> beta;
    for (int t = 0; t <= G->max_timestep; ++t) {
        VertexAppearance za{z, t};
        if (sbd.dists[z][t] == sbd.totalDists[z]) {
            beta[za] = 1.0;
        }
    }

    std::vector<VertexAppearance> topo(visited.begin(), visited.end());
    std::sort(topo.begin(), topo.end(),
        [&](auto &a, auto &b) {
            return sbd.dists[a.v][a.time] > sbd.dists[b.v][b.time];
        });

    for (auto &va : topo) {
        for (auto &w : succs[va]) {
            beta[va] += beta[w];
        }
    }

    double score = 0.0;
    for (auto &va : visited) {
        if (va.v != s && va.v != z) {
            score += (sbd.sigmas[va.v][va.time] * beta[va]) / sigma_sz;
        }
    }
    return score;
}

void compute_probs_and_scores(const TemporalGraph* G, std::unordered_map<std::pair<int, int>, double, pair_hash>& scores, std::unordered_map<std::pair<int, int>, double, pair_hash>& probs) {
    
    long long int sum_scores = 0;
    for (int s = 0; s < G->num_nodes; ++s) {
        for (int z = 0; z < G->num_nodes; ++z) {
            if (s != z) {
                scores[{s, z}] = compute_score(G, s, z);
                probs[{s, z}] = scores[{s, z}];
                sum_scores += scores[{s, z}];
            }
        }
    }

    for (auto& [pair, score] : probs) {
        score /= sum_scores;
    }

}
