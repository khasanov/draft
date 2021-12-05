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
