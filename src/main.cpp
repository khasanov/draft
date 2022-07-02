#include <vector>

#include "draft.h"

int processCommandLine(const std::vector<std::string> &args)
{
    using namespace draft;
    if (args.size() > 1) {
        return Draft::usage();
    } else if (args.size() == 1) {
        return Draft::runFile(args.at(0));
    }
    return Draft::runPrompt();
}

int main(int argc, char *argv[])
{
    std::vector<std::string> args{argv + 1, argv + argc};
    return processCommandLine(args);
}
