#include "utils.h"

void parse_command_line(int argc, char* argv[], parameters *params) {

    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-filename") {
            params->filename = argv[++i];
            continue;
        } else if (std::string(argv[i]) == "-num_samples") {
            params->num_samples = std::stoi(argv[++i]);
            continue;
        } else if (std::string(argv[i]) == "-seed") {
            params->seed = std::stoi(argv[++i]);
            continue;
        } else {
            std::cerr << "Unknown options " << std::endl;
            exit(1);
        }
    }


}