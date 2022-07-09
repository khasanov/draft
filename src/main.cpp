#include <vector>

#include "driver.h"

int processCommandLine(const std::vector<std::string> &args)
{
    using namespace draft;
    if (args.size() > 1) {
        return Driver::usage();
    } else if (args.size() == 1) {
        return Driver::runFile(args.at(0));
    }
    return Driver::runPrompt();
}

int main(int argc, char *argv[])
{
    std::vector<std::string> args{argv + 1, argv + argc};
    return processCommandLine(args);
}
