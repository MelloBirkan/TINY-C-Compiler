# Compilador TINY-C

Este é um compilador para a linguagem TINY-C, desenvolvido como projeto para a disciplina de Compiladores.

## Autores
- Marcello Gonzatto Birkan & Daniela Brazolin Flauto

## Descrição do Projeto

O compilador TINY-C implementa as fases de análise léxica e sintática de um compilador para a linguagem TINY-C, uma simplificação da linguagem C. A linguagem TINY-C possui as seguintes características:

- Tipos inteiros (int) e caractere (char)
- Comandos condicionais (if)
- Comandos de repetição (while)
- Funções de entrada (readint) e saída (writeint)

O compilador realiza a análise léxica e sintática de programas escritos em TINY-C, reportando erros e apresentando informações sobre os átomos reconhecidos durante a análise.

## Estado Atual do Projeto

O compilador está completamente implementado com as seguintes funcionalidades:

- Análise léxica completa, com reconhecimento de todos os átomos da linguagem TINY-C
- Análise sintática completa, verificando a corretude dos programas de acordo com a gramática
- Geração de mensagens de erro detalhadas em caso de problemas léxicos ou sintáticos
- Contagem de linhas do arquivo fonte e apresentação do total de linhas analisadas

## Estrutura do Projeto

O projeto está organizado da seguinte forma:

- `src/`: Código-fonte do compilador
  - `compilador.c`: Programa principal que coordena a análise
  - `lexico.c` e `lexico.h`: Implementação do analisador léxico
  - `sintatico.c` e `sintatico.h`: Implementação do analisador sintático
- `exemplos/`: Exemplos de programas em TINY-C
  - `teste1.tc`: Exemplo que encontra o maior entre dois números
  - `teste2.tc`: Exemplo simples com apenas uma função de saída
- `bin/`: Diretório onde é gerado o executável do compilador
- `Makefile`: Arquivo para automatizar a compilação
- `projeto.txt`: Documentação detalhada das especificações do projeto

## Compilação

Para compilar o compilador TINY-C, execute o comando:

```bash
make
```

Isso irá gerar o executável `bin/compilador`.

## Uso

Para utilizar o compilador TINY-C, execute:

```bash
./bin/compilador <arquivo_fonte>
```

Onde `<arquivo_fonte>` é o caminho para um arquivo fonte em TINY-C com extensão `.tc`.

## Gramática da Linguagem TINY-C

A sintaxe da linguagem TINY-C é descrita pela seguinte gramática em EBNF:

```
<program> ::= void main '(' void ')' <compound_stmt>
<compound_stmt> ::= '{' [ <var_decl> <stmt> ] '}'
<var_decl> ::= <type_specifier> <var_decl_list> ';'
<type_specifier> ::= int | char
<var_decl_list> ::= <variable_id> { ',' <variable_id> }
<variable_id> ::= id [ '=' <expr> ]
<stmt> ::= <compound_stmt> |
           <assig_stmt> |
           <cond_stmt> |
           <while_stmt> |
           readint '(' id ')' ';' |
           writeint '(' expr ')' ';'
<assig_stmt> ::= id '=' <expr> ';'
<cond_stmt> ::= if '(' expr ')' <stmt> [ else <stmt> ]
<while_stmt> ::= while '(' expr ')' <stmt>
<expr> ::= <conjunction> { '||' <conjunction> }
<conjunction> ::= <comparison> { '&&' <comparison> }
<comparison> ::= <sum> [ <relation> <sum> ]
<relation> ::= "<" | "<=" | "==" | "!=" | ">" | ">="
<sum> ::= <term> { ('+' | '-') <term> }
<term> ::= <factor> { ( '*' | '/' ) <factor> }
<factor> ::= intconst | charconst | id | '(' expr ')'
```

## Especificações Léxicas

- **Caracteres Delimitadores**: Espaços em branco, quebra de linhas, tabulação e retorno de carro são ignorados.
- **Comentários**: Existem dois tipos: comentário de linha (`//`) e comentário de bloco (`/* */`).
- **Identificadores**: Começam com uma letra ou underline, seguidos por zero ou mais letras, underlines ou dígitos, limitados a 15 caracteres.
- **Palavras Reservadas**: `char`, `else`, `if`, `int`, `main`, `readint`, `void`, `while`, `writeint`.
- **Constantes**: 
  - `charconst`: Qualquer caractere entre apóstrofos, ex: 'a', '0'.
  - `intconst`: Números inteiros na notação hexadecimal, ex: 0x1A, 0xFF.

## Saída do Compilador

- Em caso de programa sintaticamente correto:
  ```
  [linha]:[átomo]
  [linha]:[átomo]
  ...
  [N] linhas analisadas, programa sintaticamente correto
  ```

- Em caso de erro:
  ```
  [linha]:[átomo]
  [linha]:[átomo]
  ...
  [linha]:[mensagem de erro]
  ``` 