/*
 * Analisador Léxico para o compilador TINY-C
 * Implementação da fase de análise léxica
 *
 * Autores:
 * - Marcello Gonzatto Birkan
 * - Daniela Brazolin Flauto
 */

#ifndef _LEXICO_H
#define _LEXICO_H

#include <stdio.h>

// Definição dos átomos da linguagem TINY-C
typedef enum {
    ERRO,
    // Palavras reservadas
    CHAR,
    ELSE,
    IF,
    INT,
    MAIN,
    READINT,
    VOID,
    WHILE,
    WRITEINT,
    // Símbolos
    ABRE_PAR,        // (
    FECHA_PAR,       // )
    ABRE_CHAVES,     // {
    FECHA_CHAVES,    // }
    VIRGULA,         // ,
    PONTO_VIRGULA,   // ;
    IGUAL,           // =
    // Operadores relacionais
    MENOR,           // <
    MENOR_IGUAL,     // <=
    IGUAL_IGUAL,     // ==
    DIFERENTE,       // !=
    MAIOR,           // >
    MAIOR_IGUAL,     // >=
    // Operadores lógicos
    OR,              // ||
    AND,             // &&
    // Operadores aritméticos
    MAIS,            // +
    MENOS,           // -
    VEZES,           // *
    DIVIDE,          // /
    // Outros átomos
    IDENTIFICADOR,
    INTCONST,
    CHARCONST,
    COMENTARIO,
    EOS              // Fim do arquivo
} TAtomo;

// Estrutura para armazenar informações sobre o átomo reconhecido
typedef struct {
    TAtomo atomo;
    int linha;
    int valor_int;
    char valor_char;
    char lexema[16];
} TInfoAtomo;

// Função para inicializar o analisador léxico com o arquivo fonte
void inicializa_lexico(FILE *arquivo);

// Função para obter o próximo átomo do arquivo fonte
TInfoAtomo obter_atomo();

// Função para retornar o nome do átomo como string
const char* atomo_para_string(TAtomo atomo);

#endif // _LEXICO_H 