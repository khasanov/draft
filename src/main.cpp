#include <iostream>
#include <iomanip>
#include <vector>

#include <boost/filesystem.hpp>

namespace raft {

namespace exit {
constexpr auto failure = 1;  // Failing exit status.
constexpr auto success = 0;  // Successful exit status.
constexpr auto usage = 64;   // Command line usage error.
}  // namespace exit

void run(std::string_view line)
{
    std::cout << std::quoted(line) << "\n";
}

int runFile(const std::string &path)
{
    namespace fs = boost::filesystem;
    if (!fs::exists(path)) {
        std::cerr << "File not found: " << path << std::endl;
        return exit::failure;
    }

    std::string source;
    fs::load_string_file(path, source);
    run(source);

    return exit::success;
}

int runPrompt()
{
    std::cout << "Raft version 0.0.1\nPress Ctrl+d to exit\n";

    std::string line;
    while (true) {
        std::cout << ">";
        std::getline(std::cin, line);

        if (std::cin.eof()) {
            std::cout << std::endl;
            break;
        }
        run(line);
    }

    return exit::success;
}

int processCommandLine(const std::vector<std::string> &args)
{
    auto ret = exit::failure;
    if (args.size() > 1) {
        std::cerr << "Usage: raft [filename]" << std::endl;
        ret = exit::usage;
    } else if (args.size() == 1) {
        ret = raft::runFile(args.at(0));
    } else {
        ret = raft::runPrompt();
    }
    return ret;
}

}  // namespace raft

int main(int argc, char *argv[])
{
    std::vector<std::string> args(argv + 1, argv + argc);
    return raft::processCommandLine(args);
}
