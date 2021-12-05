#include <vector>

#include "raft.h"

int processCommandLine(const std::vector<std::string> &args)
{
    using namespace raft;
    if (args.size() > 1) {
        return Raft::usage();
    } else if (args.size() == 1) {
        return Raft::runFile(args.at(0));
    }
    return Raft::runPrompt();
}

int main(int argc, char *argv[])
{
    std::vector<std::string> args{argv + 1, argv + argc};
    return processCommandLine(args);
}
