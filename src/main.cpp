#include <iostream>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/locale.hpp>

#include "Scanner.h"
#include "Token.h"

namespace raft {

namespace exit {
constexpr auto failure = 1;  // Failing exit status
constexpr auto success = 0;  // Successful exit status
constexpr auto usage = 64;   // Command line usage error
}  // namespace exit

void run(std::string_view source)
{
    Scanner scanner{source};
    std::vector<Token> tokens = scanner.scanTokens();

    // For now, just print the tokens
    for (auto &token : tokens) {
        std::cout << token.toString() << std::endl;
    }
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

    // TODO consider using https://github.com/lemire/fastvalidate-utf-8
    namespace blc = boost::locale::conv;
    try {
        blc::utf_to_utf<char>(source.data(), source.data() + source.size(), blc::stop);
    } catch(blc::conversion_error &ex) {
        std::cerr << "Validation failed - non UTF-8 character" << std::endl;
        return exit::failure;
    }

    run(source);

    return exit::success;
}

int runPrompt()
{
    std::cout << "Raft version 0.0.1\nPress Ctrl+d to exit\n";

    std::string line;
    while (true) {
        std::cout << "> ";
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
    std::vector<std::string> args{argv + 1, argv + argc};
    return raft::processCommandLine(args);
}
