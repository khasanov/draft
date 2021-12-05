#include "raft.h"

#include <fstream>

#include "ast.h"
#include "ast_printer.h"
#include "parser.h"
#include "scanner.h"
#include "token.h"

namespace raft {

bool Raft::hadError = false;

int Raft::usage()
{
    out("Usage: raft [filename]", std::cerr);
    return exit::usage;
}

int Raft::runFile(const std::string &path)
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

int Raft::runPrompt()
{
    out("Raft version 0.0.1\nPress Ctrl+d to exit");

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

void Raft::run(std::string_view source)
{
    Scanner scanner{source};
    std::vector<Token> tokens = scanner.scanTokens();

    // For now, just print the tokens
    for (auto &token : tokens) {
        out(token.toString());
    }

    object::NodePool pool;
    Literal *b = new(&pool) Literal{object::String{"42"}};
    Literal *a = new(&pool) Literal{object::Number{42}};
    Token t {Token::Type::Plus, "+", object::Null{}, 1};
    Binary *c = new(&pool) Binary{b, t, a};
    AstPrinter p;
    out(p.print(c));
}

void Raft::error(std::size_t line, const std::string &message)
{
    report(line, "", message);
}

void Raft::report(std::size_t line, const std::string &where, const std::string &message)
{
    out("[line " + std::to_string(line) + "] Error " + where + ": " + message, std::cerr);
    hadError = true;
}

void Raft::out(std::string_view what, std::ostream &where)
{
    where << what << std::endl;
}
}  // namespace raft
