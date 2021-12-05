# Raft programming language

My experiments in cRAFTing a toy programming language.

## Readings

[Crafting Interpreters](craftinginterpreters.com)

## Grammar

```
expression :: literal | unary | binary | grouping
literal    :: NUMBER | STRING | "true" | "false" | "nil"
grouping   :: "(" expression ")"
unary      :: ( "-" | "!" ) expression
binary     :: expression operator expression
operator   :: "==" | "!=" | "<" | "<=" | ">" | ">=" | "+" | "-" | "*" | "/"
```

Precedence
```
expression :: equality
equality   :: comparison ( ("!=" | "==" ) comparision)*
comparison :: term ( ( ">" | ">=" | "<" | "<=" ) term )*
term       :: factor ( ("-" | "+" ) factor )*
factor     :: unary ( ( "/" | "*" ) unary )*
unary      :: ( "!" "-" ) unary | primary
primary    :: NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")"
```
