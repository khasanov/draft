#include "draft.h"

#include <fstream>

#include "ast.h"
#include "ast_printer.h"
#include "parser.h"
#include "resolver.h"
#include "scanner.h"
#include "token.h"

namespace draft {

bool Draft::hadError = false;

int Draft::usage()
{
    out("Usage: draft [filename]", std::cerr);
    return exit::usage;
}

int Draft::runFile(const std::string &path)
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
        out("Can't read file: " + path, std::cerr);
        return exit::failure;
    }

    run(source);

    if (hadError) {
        return exit::dataerr;
    }

    return exit::success;
}

int Draft::runPrompt()
{
    out("Draft version 0.0.1\nPress Ctrl+d to exit");

    std::string line;
    while (true) {
        hadError = false;  // reset error status
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
        out(p.print(stmt));
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
    out("[line " + std::to_string(line) + "] Error " + where + ": " + message, std::cerr);
}

void Draft::out(std::string_view what, std::ostream &where)
{
    where << what << std::endl;
}
}  // namespace draft
