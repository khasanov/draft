#include "draft.h"

#include <fstream>

#include "ast.h"
#include "ast_printer.h"
#include "parser.h"
#include "resolver.h"
#include "scanner.h"
#include "token.h"

namespace draft {
namespace io {

constexpr auto escapeCyan = "\x1b[36m";
constexpr auto escapeReset = "\x1b[0m";

void read(std::string &text, std::istream &stream)
{
    text.assign(std::istreambuf_iterator<char>(stream), {});
}

void write(std::string_view text, std::ostream &stream)
{
    stream << text;
}

void readLine(std::string &line, std::istream &stream)
{
    std::getline(stream, line);
}

void writeLine(std::string_view line, std::ostream &stream)
{
    stream << line << std::endl;
}
}  // namespace io

bool Draft::hadError = false;

int Draft::usage()
{
    io::writeLine("Usage: draft [filename]", std::cerr);
    return exit::usage;
}

void prompt()
{
    io::writeLine("Draft version 0.0.1");
    io::writeLine("Press Ctrl+d to exit");
}

void ps1()
{
    io::write(io::escapeCyan);
    io::write("> ");
    io::write(io::escapeReset);
}

int Draft::runFile(const std::string &path)
{
    std::ifstream file{path, std::ios::binary};
    if (file.fail()) {
        io::writeLine("Can't read file: " + path, std::cerr);
        return exit::failure;
    }
    std::string buffer;
    io::read(buffer, file);

    run(buffer);

    if (hadError) {
        return exit::dataerr;
    }

    return exit::success;
}

int Draft::runPrompt()
{
    prompt();

    std::string line;
    while (true) {
        hadError = false;  // reset error status
        ps1();
        io::readLine(line, std::cin);
        if (std::cin.eof()) {
            io::writeLine("");
            break;
        }
        run(line);
    }
    return exit::success;
}

void Draft::run(std::string_view source)
{
    Scanner scanner{source};
    std::vector<Token> tokens = scanner.scanTokens();

    Parser parser{tokens};
    std::vector<Stmt *> statements = parser.parse();
    if (hadError) {
        return;
    }

    AstPrinter p;
    for (auto stmt : statements) {
        io::writeLine(p.print(stmt));
    }

    static Interpreter interpreter;
    static Resolver resolver{&interpreter};
    resolver.resolve(statements);
    interpreter.interpret(statements);
}

void Draft::error(std::size_t line, const std::string &message)
{
    report(line, "", message);
    hadError = true;
}

void Draft::report(std::size_t line, const std::string &where, const std::string &message)
{
    io::writeLine("[line " + std::to_string(line) + "] Error " + where + ": " + message, std::cerr);
}

}  // namespace draft
