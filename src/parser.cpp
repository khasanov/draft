#include "parser.h"

#include "raft.h"

namespace raft {

template <typename Base, typename T>
inline bool instanceof (const T *ptr)
{
    return dynamic_cast<const Base *>(ptr) != nullptr;
}

RuntimeError::RuntimeError(const Token &token, const std::string &message)
    : std::runtime_error{message}
    , token{token}
{
}

/*
A recursive descent parser is a literal translation of the grammar's rules straight into imperative
code. Each rule becomes a function. The body of the rule translates to code roughtly like:

Grammar notation  Code representation
Terminal          Code to match and consule a token
Nonterminal       Call to that rule's function
|                 if or switch statement
* or +            while of for loop
?                 if statement
*/
Parser::Parser(const std::vector<Token> &tokens)
    : tokens{tokens}
{
}

// program :: declaration* EOF ;
std::vector<Stmt *> Parser::parse()
{
    std::vector<Stmt *> statements;
    try {
        while (!isAtEnd()) {
            statements.emplace_back(declaration());
        }
    } catch (const RuntimeError &ex) {
        Raft::error(ex.token.line, ex.what());
        synchronize();
    }
    return statements;
}

// expression :: assignment ;
Expr *Parser::expression()
{
    return assignment();
}

// assignment :: ( call "." )? IDENTIFIER "=" assignment | logic_or ;
Expr *Parser::assignment()
{
    Expr *expr = logicOr();
    if (match(Token::Kind::EqualsSign)) {
        Token equals = previous();
        Expr *value = assignment();

        if (instanceof <Variable>(expr)) {
            Token name = static_cast<Variable *>(expr)->name;
            return makeAstNode<Assign>(name, value);
        } else if (instanceof <Get>(expr)) {
            auto get = dynamic_cast<Get *>(expr);
            return makeAstNode<Set>(get->object, get->name, value);
        }
        Raft::error(equals.line, "Invalid assignment target");
    }
    return expr;
}

// equality :: comparison ( ( "!=" | "==" ) comparision )* ;
Expr *Parser::equality()
{
    Expr *expr = comparison();
    while (match(Token::Kind::ExclaimEqual, Token::Kind::EqualEqual)) {
        Token op = previous();
        Expr *right = comparison();
        expr = makeAstNode<Binary>(expr, op, right);
    }
    return expr;
}

// comparison :: term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
Expr *Parser::comparison()
{
    Expr *expr = term();
    while (match(Token::Kind::GreaterThanSign, Token::Kind::GreaterEqual, Token::Kind::LessThanSign,
                 Token::Kind::LessEqual)) {
        Token op = previous();
        Expr *right = term();
        expr = makeAstNode<Binary>(expr, op, right);
    }
    return expr;
}

// term :: factor ( ("-" | "+" ) factor )* ;
Expr *Parser::term()
{
    Expr *expr = factor();
    while (match(Token::Kind::HyphenMinus, Token::Kind::PlusSign)) {
        Token op = previous();
        Expr *right = factor();
        expr = makeAstNode<Binary>(expr, op, right);
    }
    return expr;
}

// factor :: unary ( ( "/" | "*" ) unary )* ;
Expr *Parser::factor()
{
    Expr *expr = unary();
    while (match(Token::Kind::Solidus, Token::Kind::Asterisk)) {
        Token op = previous();
        Expr *right = unary();
        expr = makeAstNode<Binary>(expr, op, right);
    }
    return expr;
}

// logic_or :: logic_and ( "or" logic_and )* ;
Expr *Parser::logicOr()
{
    Expr *expr = logicAnd();
    while (match(Token::Kind::Or)) {
        Token op = previous();
        Expr *right = logicAnd();
        expr = makeAstNode<Logical>(expr, op, right);
    }
    return expr;
}

// logic_and :: equality ( "and" equality )* ;
Expr *Parser::logicAnd()
{
    Expr *expr = equality();
    while (match(Token::Kind::And)) {
        Token op = previous();
        Expr *right = equality();
        expr = makeAstNode<Logical>(expr, op, right);
    }
    return expr;
}

// unary :: ( "!" | "-" ) unary | call ;
Expr *Parser::unary()
{
    if (match(Token::Kind::ExclamationMark, Token::Kind::HyphenMinus)) {
        Token op = previous();
        Expr *right = unary();
        return makeAstNode<Unary>(op, right);
    }
    return call();
}

// call :: primary ( "(" arguments? ")" | "." IDENTIFIER )* ;
Expr *Parser::call()
{
    Expr *expr = primary();

    while (true) {
        if (match(Token::Kind::LeftParenthesis)) {
            expr = finishCall(expr);
        } else if (match(Token::Kind::FullStop)) {
            Token name = consume(Token::Kind::Identifier, "Expect property name after '.'");
            expr = makeAstNode<Get>(expr, name);
        } else {
            break;
        }
    }
    return expr;
}

// arguments :: expression ( "," expression )* ;
// also handle zero-argument case
Expr *Parser::finishCall(Expr *callee)
{
    std::vector<Expr *> arguments;
    if (!check(Token::Kind::RightParenthesis)) {
        do {
            if (arguments.size() >= 255) {
                Raft::error(peek().line, "Can't have more than 255 arguments");
            }
            arguments.emplace_back(expression());
        } while (match(Token::Kind::Comma));
    }

    Token paren = consume(Token::Kind::RightParenthesis, "Expect ')' after arguments");

    return makeAstNode<Call>(callee, paren, arguments);
}

// primary :: "true" | "false" | "nil" | "this" | NUMBER | STRING | IDENTIFIER
//     | "(" expression ")" | "super" . IDENTIFIER ;
Expr *Parser::primary()
{
    if (match(Token::Kind::False)) {
        return makeAstNode<Literal>(object::Boolean{false});
    }
    if (match(Token::Kind::True)) {
        return makeAstNode<Literal>(object::Boolean{true});
    }
    if (match(Token::Kind::Nil)) {
        return makeAstNode<Literal>(object::Null{});
    }
    if (match(Token::Kind::NumberLiteral, Token::Kind::StringLiteral)) {
        return makeAstNode<Literal>(previous().literal);
    }
    if (match(Token::Kind::Super)) {
        Token keyword = previous();
        consume(Token::Kind::FullStop, "Expect '.' after 'super'");
        Token method = consume(Token::Kind::Identifier, "Expect superclass method name");
        return makeAstNode<Super>(keyword, method);
    }
    if (match(Token::Kind::This)) {
        return makeAstNode<This>(previous());
    }
    if (match(Token::Kind::Identifier)) {
        return makeAstNode<Variable>(previous());
    }

    if (match(Token::Kind::LeftParenthesis)) {
        Expr *expr = expression();
        consume(Token::Kind::RightParenthesis, "Exprect ')' after expression");
        return makeAstNode<Grouping>(expr);
    }

    throw RuntimeError{peek(), "Expect expression"};
}

// declaration :: classDecl | funDecl | varDecl | statement ;
Stmt *Parser::declaration()
{
    try {
        if (match(Token::Kind::Class)) {
            return classDeclaration();
        }
        if (match(Token::Kind::Fun)) {
            return funDeclaration();
        }
        if (match(Token::Kind::Var)) {
            return varDeclaration();
        }
        return statement();
    } catch (const RuntimeError &ex) {
        Raft::error(ex.token.line, ex.what());
        synchronize();
        return nullptr;
    }
}

// classDecl :: "class" IDENTIFIER ( "<" IDENTIFIER )? "{" function* "}" ;
Stmt *Parser::classDeclaration()
{
    Token name = consume(Token::Kind::Identifier, "Expect class name");
    Variable *superclass = nullptr;
    if (match(Token::Kind::LessThanSign)) {
        consume(Token::Kind::Identifier, "Expect superclass name");
        superclass = makeAstNode<Variable>(previous());
    }
    consume(Token::Kind::LeftCurlyBracket, "Expect '{' before class body");
    std::vector<FuncStmt *> methods;
    while (!check(Token::Kind::RightCurlyBracket) and !isAtEnd()) {
        methods.emplace_back(function("method"));
    }
    consume(Token::Kind::RightCurlyBracket, "Expect '}' after class body");
    return makeAstNode<Class>(name, superclass, methods);
}

// funDecl :: "fun" function ;
Stmt *Parser::funDeclaration()
{
    return function("function");
}

// function :: IDENTIFIER "(" parameters? ")" block ;
FuncStmt *Parser::function(std::string kind)
{
    Token name = consume(Token::Kind::Identifier, "Expect " + kind + " name");
    consume(Token::Kind::LeftParenthesis, "Expect '(' after " + kind + " name");
    std::vector<Token> parameters;
    if (!check(Token::Kind::RightParenthesis)) {
        do {
            if (parameters.size() >= 255) {
                Raft::error(peek().line, "Can't have more than 255 parameters");
            }

            parameters.push_back(consume(Token::Kind::Identifier, "Exprect parameter name"));
        } while (match(Token::Kind::Comma));
    }
    consume(Token::Kind::RightParenthesis, "Expect ')' after parameters");
    consume(Token::Kind::LeftCurlyBracket, "Expect '{' before " + kind + " body");
    std::vector<Stmt *> body = block();
    return makeAstNode<FuncStmt>(name, parameters, body);
}

// varDecl :: "var" IDENTIFIER ( "=" expression )? ";" ;
Stmt *Parser::varDeclaration()
{
    Token name = consume(Token::Kind::Identifier, "Expect variable name");
    Expr *initializer = nullptr;
    if (match(Token::Kind::EqualsSign)) {
        initializer = expression();
    }
    consume(Token::Kind::Semicolon, "Expect ';' after variable declaration");
    return makeAstNode<Var>(name, initializer);
}

// statement :: exprStmt | forStmt | ifStmt | printStmt | returnStmt | whileStmt | block ;
Stmt *Parser::statement()
{
    if (match(Token::Kind::For)) {
        return forStatement();
    }
    if (match(Token::Kind::If)) {
        return ifStatement();
    }
    if (match(Token::Kind::Print)) {
        return printStatement();
    }
    if (match(Token::Kind::Return)) {
        return returnStatement();
    }
    if (match(Token::Kind::While)) {
        return whileStatement();
    }
    if (match(Token::Kind::LeftCurlyBracket)) {
        return makeAstNode<Block>(block());
    }
    return expressionStatement();
}

// forStmt :: "for" "(" ( varDecl | exprStmt | ";" ) expression? ";" expression? ")" statement ;
Stmt *Parser::forStatement()
{
    consume(Token::Kind::LeftParenthesis, "Expect '(' after 'for'");

    Stmt *initializer = nullptr;
    if (match(Token::Kind::Semicolon)) {
        initializer = nullptr;
    } else if (match(Token::Kind::Var)) {
        initializer = varDeclaration();
    } else {
        initializer = expressionStatement();
    }

    Expr *condition = nullptr;
    if (!check(Token::Kind::Semicolon)) {
        condition = expression();
    }
    consume(Token::Kind::Semicolon, "Expect ';' after loop condition");

    Expr *increment = nullptr;
    if (!check(Token::Kind::RightParenthesis)) {
        increment = expression();
    }
    consume(Token::Kind::RightParenthesis, "Expect ')' after for clauses");

    Stmt *body = statement();

    /* desugaring for statement to while statement
    for(<init>; <cond>; <iter>) <body> is equvivalent to

    <init>
    while(<cond>) {
      <body>
      <iter>
    }
    */

    if (increment != nullptr) {
        std::vector<Stmt *> stmts;
        stmts.emplace_back(body);
        stmts.emplace_back(makeAstNode<ExprStmt>(increment));
        body = makeAstNode<Block>(stmts);
    }
    if (condition == nullptr) {
        condition = makeAstNode<Literal>(object::Boolean{true});
    }
    body = makeAstNode<While>(condition, body);

    if (initializer) {
        std::vector<Stmt *> stmts;
        stmts.emplace_back(initializer);
        stmts.emplace_back(body);
        body = makeAstNode<Block>(std::move(stmts));
    }
    return body;
}

// ifStmt :: "if" "(" expression ")" statement ( "else" statement )? ;
Stmt *Parser::ifStatement()
{
    consume(Token::Kind::LeftParenthesis, "Exprect '(' after 'if'");
    Expr *condition = expression();
    consume(Token::Kind::RightParenthesis, "Exprect ')' after if condition");

    Stmt *thenBranch = statement();
    Stmt *elseBranch = nullptr;
    if (match(Token::Kind::Else)) {
        elseBranch = statement();
    }
    return makeAstNode<If>(condition, thenBranch, elseBranch);
}

// printStmt :: "print" expression ";" ;
Stmt *Parser::printStatement()
{
    Expr *value = expression();
    consume(Token::Kind::Semicolon, "Expect ';' after value");
    return makeAstNode<Print>(value);
}

// returnStmt :: "return" expression? ";" ;
Stmt *Parser::returnStatement()
{
    Token keyword = previous();
    Expr *value = nullptr;
    if (!check(Token::Kind::Semicolon)) {
        value = expression();
    }
    consume(Token::Kind::Semicolon, "Expect ';' after return value");
    return makeAstNode<Return>(keyword, value);
}

// whileStmt :: "while" "(" expression ")" statement ;
Stmt *Parser::whileStatement()
{
    consume(Token::Kind::LeftParenthesis, "Expect '(' after 'while'");
    Expr *condition = expression();
    consume(Token::Kind::RightParenthesis, "Exprect ')' after condition");
    Stmt *body = statement();

    return makeAstNode<While>(condition, body);
}

// block :: "{" declaration* "}" ;
std::vector<Stmt *> Parser::block()
{
    std::vector<Stmt *> statements;
    while (!check(Token::Kind::RightCurlyBracket) and !isAtEnd()) {
        statements.emplace_back(declaration());
    }
    consume(Token::Kind::RightCurlyBracket, "Exprect '}' after block");
    return statements;
}

// exprStmt :: expresson ";" ;
Stmt *Parser::expressionStatement()
{
    Expr *expr = expression();
    consume(Token::Kind::Semicolon, "Exprect ';' after expession");
    return makeAstNode<ExprStmt>(expr);
}

bool Parser::isAtEnd()
{
    return peek().kind == Token::Kind::EndOfFile;
}

Token Parser::peek()
{
    return tokens.at(current);
}

Token Parser::previous()
{
    return tokens.at(current - 1);
}

Token Parser::advance()
{
    if (!isAtEnd()) {
        current++;
    }
    return previous();
}

bool Parser::check(Token::Kind kind)
{
    if (isAtEnd()) {
        return false;
    }
    return peek().kind == kind;
}

Token Parser::consume(Token::Kind kind, const std::string &message)
{
    if (check(kind)) {
        return advance();
    }
    throw RuntimeError{peek(), message};
}

void Parser::synchronize()
{
    advance();

    while (!isAtEnd()) {
        if (previous().kind == Token::Kind::Semicolon) {
            return;
        }

        switch (peek().kind) {
        case Token::Kind::Class:
            [[fallthrough]];
        case Token::Kind::Fun:
            [[fallthrough]];
        case Token::Kind::Var:
            [[fallthrough]];
        case Token::Kind::For:
            [[fallthrough]];
        case Token::Kind::If:
            [[fallthrough]];
        case Token::Kind::While:
            [[fallthrough]];
        case Token::Kind::Print:
            [[fallthrough]];
        case Token::Kind::Return:
            return;
        default:
            break;
        }

        advance();
    }
}

}  // namespace raft
