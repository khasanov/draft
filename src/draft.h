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

namespace io {
void read(std::string &text, std::istream &stream = std::cin);
void write(std::string_view text, std::ostream &stream = std::cout);
void readLine(std::string &line, std::istream &stream = std::cin);
void writeLine(std::string_view line, std::ostream &stream = std::cout);
}  // namespace io

class Draft {
public:
    static int usage();

    static int runFile(const std::string &path);

    static int runPrompt();

    static void error(std::size_t line, const std::string &message);
    static void report(std::size_t line, const std::string &where, const std::string &message);

    static void run(std::string_view source);

private:
    static bool hadError;
};

}  // namespace draft
