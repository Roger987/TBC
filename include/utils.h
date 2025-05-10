#ifndef UTILS_H
#define UTILS_H

#include <utility>     
#include <functional>  
#include <iostream>

struct pair_hash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
    }
};

struct parameters {
    std::string filename;
    int num_samples;
    int seed;
};

void parse_command_line(int argc, char* argv[], parameters *params);

#endif // UTILS_H