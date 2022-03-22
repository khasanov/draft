# Raft programming language

My experiments in cRAFTing a toy programming language.

## Readings

[Crafting Interpreters](craftinginterpreters.com)

## Grammar

```
program     :: declaration* EOF
declaration :: classDecl | funDecl | varDecl | statement
statement   :: exprStmt | forStmt | ifStmt | printStmt | returnStmt | whileStmt | block
exprStmt    :: expresson ";"
forStmt     :: "for" "(" (varDecl | exprStmt | ";" ) expression? ";" expression? ")" statement
ifStmt      :: "if" "(" expression ")" statement ( "else" statement )?
printStmt   :: "print" expression ";"
returnStmt  :: "return" expression? ";"
whileStmt   :: "while" "(" expression ")" statement
block       :: "{" declaration* "}"
classDecl   :: "class" IDENTIFIER "{" function* "}"
funDecl     :: "fun" function
function    :: IDENTIFIER "(" parameters? ")" block
parameters  :: IDENTIFIER ( "," IDENTIFIER )*
varDecl     :: "var" IDENTIFIER ( "=" expression )? ";"

expression  :: assignment
assignment  :: IDENTIFIER "=" assignment | logic_or
logic_or    :: logic_and ( "or" logic_and )*
logic_and   :: equality ( "and" equality )*
equality    :: comparison ( ("!=" | "==" ) comparision)*
comparison  :: term ( ( ">" | ">=" | "<" | "<=" ) term )*
term        :: factor ( ("-" | "+" ) factor )*
factor      :: unary ( ( "/" | "*" ) unary )*
unary       :: ( "!" "-" ) unary | call
call        :: primary ( "(" arguments? ")" | "." IDENTIFIER )*
arguments   :: expression ( "," expression )*
primary     :: NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" | IDENTIFIER
```

## Ast representation

```
Expr{}                      for expression
Lit{value}                  for literals: NUMBER, STRING, "true", "false", "nil"
UnOp{op, Expr{}}            for unary
BinOp{op, Expr{}, Expr{}}   for binary
{Expr{}}                    for grouping: "(" expression ")"

ExprStmt{Expr{}}            for expression statement
PrintStmt{Expr{}}           for print statement
```
