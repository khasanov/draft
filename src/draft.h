#pragma once

#include <iostream>

#include "interpreter.h"

namespace draft {

namespace exit {
constexpr int success = 0;    // Successful exit status
constexpr int failure = 1;    // Failing exit status
constexpr int usage = 64;     // Command line usage error
constexpr int dataerr = 65;   // Incorrect input data
constexpr int software = 70;  // Internal software error
}  // namespace exit

class Draft {
public:
    static int usage();

    static int runFile(const std::string &path);

    static int runPrompt();

    static void error(std::size_t line, const std::string &message);
    static void report(std::size_t line, const std::string &where, const std::string &message);
    static void out(std::string_view what, std::ostream &where = std::cout);

    static void run(std::string_view source);

private:
    static bool hadError;
};

}  // namespace draft