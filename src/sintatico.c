/*
 * Analisador Sintático para o compilador TINY-C
 * Implementação da fase de análise sintática
 *
 * Autores:
 * - Marcello Gonzatto Birkan
 * - Daniela Brazolin Flauto
 */

#include "sintatico.h"
#include <stdio.h>
#include <stdlib.h>

// Variáveis globais
static TAtomo lookahead;
static TInfoAtomo info_atomo;

// Inicializa o analisador sintático
void inicializa_sintatico() {
  // Obtém o primeiro átomo não-comentário
  do {
    info_atomo = obter_atomo();
    if (info_atomo.atomo == COMENTARIO) {
      printf("# %d:comentario\n", info_atomo.linha);
    }
  } while (info_atomo.atomo == COMENTARIO);

  lookahead = info_atomo.atomo;

  // Verifica se já começa com um erro léxico
  if (lookahead == ERRO) {
    exit(1);
  }
}

// Função para consumir o átomo atual e solicitar o próximo
void consome(TAtomo atomo_esperado) {
  // Verifica se o token atual é um ERRO e encerra o programa
  if (lookahead == ERRO) {
    // Não é necessário mostrar uma mensagem aqui, pois o analisador léxico já
    // mostrou
    exit(1);
  }

  if (lookahead == atomo_esperado) {
    // Imprime o átomo atual
    printf("# %d:%s", info_atomo.linha, atomo_para_string(info_atomo.atomo));

    // Adiciona informações extras dependendo do tipo de átomo
    if (lookahead == IDENTIFICADOR) {
      printf(" | %s", info_atomo.lexema);
    } else if (lookahead == INTCONST) {
      printf(" | %d", info_atomo.valor_int);
    } else if (lookahead == CHARCONST) {
      printf(" | '%c'", info_atomo.valor_char);
    }

    printf("\n");

    // Obtém o próximo átomo, ignorando comentários
    do {
      info_atomo = obter_atomo();
      if (info_atomo.atomo == COMENTARIO) {
        printf("# %d:comentario\n", info_atomo.linha);
      }
    } while (info_atomo.atomo == COMENTARIO);

    lookahead = info_atomo.atomo;
  } else {
    // Erro sintático
    printf("# %d:erro sintatico, esperado [%s] encontrado [%s]\n",
           info_atomo.linha, atomo_para_string(atomo_esperado),
           atomo_para_string(lookahead));
    exit(1);
  }
}

// <program> ::= void main '(' void ')' <compound_stmt>
void program() {
  consome(VOID);
  consome(MAIN);
  consome(ABRE_PAR);
  consome(VOID);
  consome(FECHA_PAR);
  compound_stmt();
}

// <compound_stmt> ::= '{' [ <var_decl> <stmt> ] '}'
void compound_stmt() {
  consome(ABRE_CHAVES);

  // Verifica se há declarações de variáveis ou instruções
  while (lookahead != FECHA_CHAVES && lookahead != EOS) {
    // Declarações de variáveis
    if (lookahead == INT || lookahead == CHAR) {
      var_decl();
    } else {
      // Instruções
      stmt();
    }
  }

  consome(FECHA_CHAVES);
}

// <var_decl> ::= <type_specifier> <var_decl_list> ';'
void var_decl() {
  type_specifier();
  var_decl_list();
  consome(PONTO_VIRGULA);
}

// <type_specifier> ::= int | char
void type_specifier() {
  if (lookahead == INT) {
    consome(INT);
  } else if (lookahead == CHAR) {
    consome(CHAR);
  } else {
    consome(INT); // Vai gerar um erro se não for INT nem CHAR
  }
}

// <var_decl_list> ::= <variable_id> { ',' <variable_id> }
void var_decl_list() {
  variable_id();

  while (lookahead == VIRGULA) {
    consome(VIRGULA);
    variable_id();
  }
}

// <variable_id> ::= id [ '=' <expr> ]
void variable_id() {
  consome(IDENTIFICADOR);

  if (lookahead == IGUAL) {
    consome(IGUAL);
    expr();
  }
}

// <stmt> ::= <compound_stmt> | <assig_stmt> | <cond_stmt> | <while_stmt> |
// readint '(' id ')' ';' | writeint '(' expr ')' ';'
void stmt() {
  switch (lookahead) {
  case ABRE_CHAVES:
    compound_stmt();
    break;

  case IDENTIFICADOR:
    assig_stmt();
    break;

  case IF:
    cond_stmt();
    break;

  case WHILE:
    while_stmt();
    break;

  case READINT:
    consome(READINT);
    consome(ABRE_PAR);
    consome(IDENTIFICADOR);
    consome(FECHA_PAR);
    consome(PONTO_VIRGULA);
    break;

  case WRITEINT:
    consome(WRITEINT);
    consome(ABRE_PAR);
    expr();
    consome(FECHA_PAR);
    consome(PONTO_VIRGULA);
    break;

  default:
    // Erro sintático
    printf("# %d:erro sintatico, esperava início de comando\n",
           info_atomo.linha);
    exit(1);
  }
}

// <assig_stmt> ::= id '=' <expr> ';'
void assig_stmt() {
  consome(IDENTIFICADOR);
  consome(IGUAL);
  expr();
  consome(PONTO_VIRGULA);
}

// <cond_stmt> ::= if '(' expr ')' <stmt> [ else <stmt> ]
void cond_stmt() {
  consome(IF);
  consome(ABRE_PAR);
  expr();
  consome(FECHA_PAR);
  stmt();

  if (lookahead == ELSE) {
    consome(ELSE);
    stmt();
  }
}

// <while_stmt> ::= while '(' expr ')' <stmt>
void while_stmt() {
  consome(WHILE);
  consome(ABRE_PAR);
  expr();
  consome(FECHA_PAR);
  stmt();
}

// <expr> ::= <conjunction> { '||' <conjunction> }
void expr() {
  conjunction();

  while (lookahead == OR) {
    consome(OR);
    conjunction();
  }
}

// <conjunction> ::= <comparison> { '&&' <comparison> }
void conjunction() {
  comparison();

  while (lookahead == AND) {
    consome(AND);
    comparison();
  }
}

// <comparison> ::= <sum> [ <relation> <sum> ]
void comparison() {
  sum();

  if (lookahead == MENOR || lookahead == MENOR_IGUAL ||
      lookahead == IGUAL_IGUAL || lookahead == DIFERENTE ||
      lookahead == MAIOR || lookahead == MAIOR_IGUAL) {
    relation();
    sum();
  }
}

// <relation> ::= "<" | "<=" | "==" | "!=" | ">" | ">="
void relation() {
  switch (lookahead) {
  case MENOR:
    consome(MENOR);
    break;

  case MENOR_IGUAL:
    consome(MENOR_IGUAL);
    break;

  case IGUAL_IGUAL:
    consome(IGUAL_IGUAL);
    break;

  case DIFERENTE:
    consome(DIFERENTE);
    break;

  case MAIOR:
    consome(MAIOR);
    break;

  case MAIOR_IGUAL:
    consome(MAIOR_IGUAL);
    break;

  default:
    // Erro sintático
    printf("# %d:erro sintatico, esperava operador relacional\n",
           info_atomo.linha);
    exit(1);
  }
}

// <sum> ::= <term> { ('+' | '-') <term> }
void sum() {
  term();

  while (lookahead == MAIS || lookahead == MENOS) {
    if (lookahead == MAIS) {
      consome(MAIS);
    } else {
      consome(MENOS);
    }
    term();
  }
}

// <term> ::= <factor> { ( '*' | '/' ) <factor> }
void term() {
  factor();

  while (lookahead == VEZES || lookahead == DIVIDE) {
    if (lookahead == VEZES) {
      consome(VEZES);
    } else {
      consome(DIVIDE);
    }
    factor();
  }
}

// <factor> ::= intconst | charconst | id | '(' expr ')'
void factor() {
  switch (lookahead) {
  case INTCONST:
    consome(INTCONST);
    break;

  case CHARCONST:
    consome(CHARCONST);
    break;

  case IDENTIFICADOR:
    consome(IDENTIFICADOR);
    break;

  case ABRE_PAR:
    consome(ABRE_PAR);
    expr();
    consome(FECHA_PAR);
    break;

  default:
    // Erro sintático
    printf("# %d:erro sintatico, esperava fator\n", info_atomo.linha);
    exit(1);
  }
}