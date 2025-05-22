# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build and Run Commands

### Build the TINY-C Compiler
```bash
make
```

### Clean the Project
```bash
make clean
```

### Run the Compiler
```bash
./bin/compilador <filename>
```
Example:
```bash
./bin/compilador exemplos/testes_fase_1/teste1.tc
```

## Architecture Overview

The TINY-C compiler implements all phases of compilation for the TINY-C language (a simplified version of C):

### Phase 1: Lexical and Syntactic Analysis
1. **Main Program (`compilador.c`)**: Coordinates the compilation process, manages analyzers, and controls the output based on analysis results
2. **Lexical Analyzer (`lexico.c`, `lexico.h`)**: Tokenizes source code and identifies atoms (tokens)
3. **Syntactic Analyzer (`sintatico.c`, `sintatico.h`)**: Implements recursive descent parser following TINY-C grammar

### Phase 2: Semantic Analysis and Code Generation
4. **Semantic Analyzer (`semantico.c`, `semantico.h`)**: 
   - Manages symbol table as a linked list (TNo structure)
   - Validates variable declarations (no duplicates)
   - Verifies variable usage (must be declared before use)
   
5. **Code Generator (`gerador.c`, `gerador.h`)**: 
   - Generates MEPA (simplified assembly) intermediate code
   - Provides functions for labels, memory allocation, arithmetic/logical operations, and control flow

## Key Implementation Details

### Global Variables and Communication
- `lookahead`: Current token being analyzed
- `InfoAtomo`: Global TInfoAtomo structure containing current token attributes
- Symbol table is accessed globally through linked list starting pointer

### Error Handling
- Stops at first error (lexical, syntactic, or semantic)
- Provides line number and descriptive error messages
- No error recovery mechanism

### Code Generation Integration
- Code generation happens during syntactic analysis (syntax-directed translation)
- Uses `proximo_rotulo()` for generating sequential labels (L1, L2, ...)
- Uses `busca_tabela_simbolos()` to get variable addresses during code generation

## TINY-C Language Features

### Supported Types
- `int`: Integer type (hexadecimal constants like 0x1A)
- `char`: Character type (single characters like 'a')

### Control Structures
- `if/else`: Conditional statements
- `while`: Loop statements

### I/O Functions
- `readint(id)`: Read integer input
- `writeint(expr)`: Write integer output

### Limitations
- Only integer expressions (no mixed type expressions)
- No functions beyond main
- No arrays or pointers
- Identifiers limited to 15 characters

## Output Format

### Successful Compilation
```
INPP
[MEPA instructions...]
PARA
TABELA DE SIMBOLOS
[variable] | Endereco: [address]
...
```

### Compilation Error
```
[line number]: [error message]
```

## Test Files Structure

- `exemplos/testes_fase_1/`: Phase 1 tests (lexical/syntactic)
- `exemplos/testes_fase_2/`: Phase 2 tests (semantic/code generation)
- `exemplos/testes_luba/`: Additional test cases

## Important Notes

- Variable addresses are assigned sequentially starting from 0
- MEPA code uses labels (L1, L2, ...) for control flow
- Symbol table must be implemented as linked list per specification
- Compiler returns 0 on success, non-zero on error