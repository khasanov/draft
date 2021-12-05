#pragma once

#include <string_view>

#include "scanner.h"
#include "token.h"

namespace raft {

namespace exit {
constexpr int failure = 1;  // Failing exit status
constexpr int success = 0;  // Successful exit status
constexpr int usage = 64;   // Command line usage error
}  // namespace exit

class Raft {
public:
    static int usage();

    static void run(std::string_view source);

    static int runFile(const std::string &path);

    static int runPrompt();
};

}  // namespace raft
