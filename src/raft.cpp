#include "raft.h"

#include <fstream>
#include <iostream>

int raft::Raft::usage()
{
    std::cerr << "Usage: raft [filename]" << std::endl;
    return exit::usage;
}

void raft::Raft::run(std::string_view source)
{
    Scanner scanner{source};
    std::vector<Token> tokens = scanner.scanTokens();

    // For now, just print the tokens
    for (auto &token : tokens) {
        std::cout << token.toString() << std::endl;
    }
}

int raft::Raft::runFile(const std::string &path)
{
    std::string source;
    try {
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        file.open(path, std::ios_base::binary);
        file.seekg(0, std::ios::end);
        const auto size = file.tellg();
        source.resize(size, '\0');
        file.seekg(0, std::ios::beg);
        file.read(&source[0], size);
    } catch (...) {
        std::cerr << "Can't read file: " << path << std::endl;
        return exit::failure;
    }

    run(source);

    return exit::success;
}

int raft::Raft::runPrompt()
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
