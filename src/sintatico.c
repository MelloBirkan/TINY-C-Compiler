/*
 * Analisador Sintático para o compilador TINY-C
 * Implementação da fase de análise sintática, semântica e geração de código
 *
 * Autores:
 * - Marcello Gonzatto Birkan
 * - Daniela Brazolin Flauto
 */

#include "sintatico.h"
#include "semantico.h"
#include "gerador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Variáveis globais
static TAtomo lookahead;
static TInfoAtomo info_atomo;
static int num_variaveis = 0;

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
    // Imprime o átomo atual (comentado para não interferir na saída MEPA)
    /*
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
    */

    // Obtém o próximo átomo, ignorando comentários
    do {
      info_atomo = obter_atomo();
      if (info_atomo.atomo == COMENTARIO) {
        // printf("# %d:comentario\n", info_atomo.linha);
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
  gera_inicio_programa();  // INPP
  
  consome(VOID);
  consome(MAIN);
  consome(ABRE_PAR);
  consome(VOID);
  consome(FECHA_PAR);
  compound_stmt();
  
  gera_fim_programa();     // PARA
  
  // Imprime a tabela de símbolos
  imprime_tabela_simbolos();
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
  // Conta o número de variáveis declaradas antes de começar
  int variaveis_iniciais = num_variaveis;
  
  variable_id();
  num_variaveis++;  // Incrementa contador de variáveis

  while (lookahead == VIRGULA) {
    consome(VIRGULA);
    variable_id();
    num_variaveis++;  // Incrementa contador de variáveis
  }
  
  // Gera código para alocar memória para as variáveis declaradas nesta lista
  gera_alocacao_memoria(num_variaveis - variaveis_iniciais);
}

// <variable_id> ::= id [ '=' <expr> ]
void variable_id() {
  char id[16];
  strcpy(id, info_atomo.lexema);  // Salva o nome da variável
  
  consome(IDENTIFICADOR);
  
  // Verifica se o ID já existe e adiciona à tabela de símbolos
  if (!insere_tabela_simbolos(id)) {
    printf("# %d:erro semantico, variavel '%s' ja declarada\n", info_atomo.linha, id);
    exit(1);
  }
  
  int endereco = busca_tabela_simbolos(id);  // Obtém o endereço da variável

  if (lookahead == IGUAL) {
    consome(IGUAL);
    expr();
    gera_armazenar_valor(endereco);  // ARMZ
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
    
    // Verifica se a variável existe e obtém seu endereço
    char id[16];
    strcpy(id, info_atomo.lexema);
    consome(IDENTIFICADOR);
    
    int endereco = busca_tabela_simbolos(id);
    if (endereco == -1) {
      printf("# %d:erro semantico, variavel '%s' nao declarada\n", info_atomo.linha, id);
      exit(1);
    }
    
    consome(FECHA_PAR);
    consome(PONTO_VIRGULA);
    
    gera_entrada();              // LEIT
    gera_armazenar_valor(endereco);  // ARMZ
    break;

  case WRITEINT:
    consome(WRITEINT);
    consome(ABRE_PAR);
    expr();
    consome(FECHA_PAR);
    consome(PONTO_VIRGULA);
    
    gera_saida();  // IMPR
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
  char id[16];
  strcpy(id, info_atomo.lexema);  // Salva o nome da variável
  
  consome(IDENTIFICADOR);
  
  // Verifica se a variável existe e obtém seu endereço
  int endereco = busca_tabela_simbolos(id);
  if (endereco == -1) {
    printf("# %d:erro semantico, variavel '%s' nao declarada\n", info_atomo.linha, id);
    exit(1);
  }
  
  consome(IGUAL);
  expr();
  consome(PONTO_VIRGULA);
  
  gera_armazenar_valor(endereco);  // ARMZ
}

// <cond_stmt> ::= if '(' expr ')' <stmt> [ else <stmt> ]
void cond_stmt() {
  int L1 = proximo_rotulo();
  int L2 = proximo_rotulo();
  
  consome(IF);
  consome(ABRE_PAR);
  expr();
  consome(FECHA_PAR);
  
  gera_desvio_se_falso(L1);  // DSVF L1
  
  stmt();
  
  gera_desvio_incondicional(L2);  // DSVS L2
  gera_nada(L1);  // L1: NADA
  
  if (lookahead == ELSE) {
    consome(ELSE);
    stmt();
  }
  
  gera_nada(L2);  // L2: NADA
}

// <while_stmt> ::= while '(' expr ')' <stmt>
void while_stmt() {
  int L1 = proximo_rotulo();
  int L2 = proximo_rotulo();
  
  gera_nada(L1);  // L1: NADA
  
  consome(WHILE);
  consome(ABRE_PAR);
  expr();
  consome(FECHA_PAR);
  
  gera_desvio_se_falso(L2);  // DSVF L2
  
  stmt();
  
  gera_desvio_incondicional(L1);  // DSVS L1
  gera_nada(L2);  // L2: NADA
}

// <expr> ::= <conjunction> { '||' <conjunction> }
void expr() {
  conjunction();

  while (lookahead == OR) {
    consome(OR);
    conjunction();
    gera_ou();  // DISJ
  }
}

// <conjunction> ::= <comparison> { '&&' <comparison> }
void conjunction() {
  comparison();

  while (lookahead == AND) {
    consome(AND);
    comparison();
    gera_e();  // CONJ
  }
}

// <comparison> ::= <sum> [ <relation> <sum> ]
void comparison() {
  sum();

  if (lookahead == MENOR || lookahead == MENOR_IGUAL ||
      lookahead == IGUAL_IGUAL || lookahead == DIFERENTE ||
      lookahead == MAIOR || lookahead == MAIOR_IGUAL) {
    
    TAtomo op = lookahead;  // Salva o operador de comparação
    
    relation();
    sum();
    
    // Gera código para a operação de comparação adequada
    switch (op) {
      case MENOR:
        gera_menor();  // CMME
        break;
      case MENOR_IGUAL:
        gera_menor_igual();  // CMEG
        break;
      case IGUAL_IGUAL:
        gera_igual();  // CMIG
        break;
      case DIFERENTE:
        gera_diferente();  // CMDG
        break;
      case MAIOR:
        gera_maior();  // CMMA
        break;
      case MAIOR_IGUAL:
        gera_maior_igual();  // CMAG
        break;
      default:
        // Este caso não deveria ocorrer, pois já verificamos acima
        // se o operador é um dos operadores de comparação válidos
        printf("# Erro interno: operador relacional não reconhecido\n");
        exit(1);
        break;
    }
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
      term();
      gera_soma();  // SOMA
    } else {
      consome(MENOS);
      term();
      gera_subtracao();  // SUBT
    }
  }
}

// <term> ::= <factor> { ( '*' | '/' ) <factor> }
void term() {
  factor();

  while (lookahead == VEZES || lookahead == DIVIDE) {
    if (lookahead == VEZES) {
      consome(VEZES);
      factor();
      gera_multiplicacao();  // MULT
    } else {
      consome(DIVIDE);
      factor();
      gera_divisao();  // DIVI
    }
  }
}

// <factor> ::= intconst | charconst | id | '(' expr ')'
void factor() {
  switch (lookahead) {
  case INTCONST:
    gera_carregar_constante(info_atomo.valor_int);  // CRCT
    consome(INTCONST);
    break;

  case CHARCONST:
    gera_carregar_constante((int)info_atomo.valor_char);  // CRCT para char
    consome(CHARCONST);
    break;

  case IDENTIFICADOR: {
    char id[16];
    strcpy(id, info_atomo.lexema);  // Salva o nome da variável
    
    // Verifica se a variável existe e obtém seu endereço
    int endereco = busca_tabela_simbolos(id);
    if (endereco == -1) {
      printf("# %d:erro semantico, variavel '%s' nao declarada\n", info_atomo.linha, id);
      exit(1);
    }
    
    gera_carregar_valor(endereco);  // CRVL
    consome(IDENTIFICADOR);
    break;
  }

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