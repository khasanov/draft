#include "driver.h"

#include <fstream>

#include "ast.h"
#include "ast_printer.h"
#include "lexer.h"
#include "parser.h"
#include "resolver.h"
#include "source_manager.h"
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

void writeColoredLine(const std::string &line)
{
    io::write(io::escapeCyan);
    io::writeLine(line);
    io::write(io::escapeReset);
}

}  // namespace io

bool Driver::hadError = false;

int Driver::usage()
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

int Driver::runFile(const std::string &path)
{
    std::ifstream file{path, std::ios::binary};
    if (file.fail()) {
        io::writeLine("Can't read file: " + path, std::cerr);
        return exit::failure;
    }
    std::string buffer;
    io::read(buffer, file);

    run(buffer, path);

    if (hadError) {
        return exit::dataerr;
    }

    return exit::success;
}

int Driver::runPrompt()
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

void Driver::run(const std::string& buffer, const std::string &path)
{
    SourceManager manager;
    auto id = manager.makeSource(buffer, path);

    if (!path.empty()) {
        io::writeColoredLine("-- " + manager.getPath(id));
    }

    Lexer lexer{buffer};
    std::vector<Token> tokens = lexer.scanTokens();

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

void Driver::error(std::size_t line, const std::string &message)
{
    report(line, "", message);
    hadError = true;
}

void Driver::report(std::size_t line, const std::string &where, const std::string &message)
{
    io::writeLine("[line " + std::to_string(line) + "] Error " + where + ": " + message, std::cerr);
}

}  // namespace draft
