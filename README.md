# Compiler Front End for an Urdu-Keyword Language

The front end of a compiler for a small C-like programming language whose keywords are in Urdu. It is written in C++ and has three stages: a lexical analyser, a recursive-descent parser, and a three-address-code generator.

Built for **Compiler Construction**, FAST-NUCES Lahore, Spring 2025.

> **Note:** This is coursework I completed in Spring 2025. I uploaded it in November 2026 to document my past work, and the code is kept as it was originally submitted.

## The language

| Keyword | Meaning |
|---|---|
| `Adadi` | integer |
| `Ashriya` | floating-point number |
| `Harf` | character |
| `Matn` | string |
| `Mantiqi` | boolean (`True`, `False`) |
| `Agar` / `Wagarna` | if / else |
| `Wapas` | return |
| `Marqazi` | the main function |
| `input->` / `output<-` | read input / print output |

Statements end with `::`. A sample program:

```
Adadi Marqazi ()
{
    Adadi _num = 10 ::
    Ashriya num_ = -10.5E+12 ::
    Mantiqi _flag = True ::
    Agar ( _flag )
        output<- "Ok" ::
    Wagarna
        input-> _num ::
    Wapas 0 ::
}
```

## Stage 1: Lexical analyser (`lexer.cpp`)

- Removes single-line and multi-line comments and extra whitespace before scanning.
- Scans the source with a **table-driven finite automaton** of 47 states over 33 character classes. The transition diagram is in `transition_table.pdf`.
- Recognises keywords, identifiers, operators, punctuation, strings, characters and numbers, including signed and scientific-notation numbers such as `-10.5E+12`.
- Reads the source through a fixed-size input buffer.
- Builds a **symbol table** for identifiers and a **literal table** for string and character literals.
- Writes every token as `<lexeme,class>` and logs invalid lexemes to `errors.txt`.

## Stage 2: Parser (`parser.cpp`)

- A **recursive-descent parser** for an LL(1) grammar, with one function for each non-terminal.
- Handles function definitions with arguments, declarations, `Agar`/`Wagarna`, `for` and `while` loops, compound statements, assignments, and relational and arithmetic expressions with correct precedence.
- Prints the **parse tree** with indentation while parsing, and reports the expected and actual token on a syntax error.

## Stage 3: Three-address code (`tac_generator.cpp`)

- Extends the parser with semantic actions defined by a syntax-directed translation scheme (`3ac_semantic_rules.txt`).
- Generates three-address code for assignments, arithmetic expressions, `Agar`/`Wagarna` and `while` loops, using temporaries (`t0`, `t1`, ...) and labels (`L0`, `L1`, ...) with conditional and unconditional jumps.

## Build and run

Each stage is a single C++ file:

```bash
g++ -std=c++17 lexer.cpp -o lexer
g++ -std=c++17 parser.cpp -o parser
g++ -std=c++17 tac_generator.cpp -o tac_generator
```

1. Put the source program in `input.txt` and run `./lexer`. It writes `lexemes.txt`, `symbolTable.txt`, `literalTable.txt` and `errors.txt`.
2. The parser and the code generator read their tokens from `tokens.txt`, in the same `<lexeme,class>` format. Run `./parser` to print the parse tree, or `./tac_generator` to print the three-address code.

## Repository contents

| File | Description |
|---|---|
| `lexer.cpp` | Lexical analyser |
| `parser.cpp` | Recursive-descent parser |
| `tac_generator.cpp` | Three-address-code generator |
| `3ac_semantic_rules.txt` | Translation scheme used by the code generator |
| `transition_table.pdf` | Transition diagram of the lexer's automaton |
| `input.txt`, `tokens.txt` | Sample source program and sample token stream |
| `lexemes.txt`, `symbolTable.txt`, `literalTable.txt` | Sample lexer output |

## Authors

Hamzah Javid.
