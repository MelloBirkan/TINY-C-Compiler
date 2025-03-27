/*
 * Analisador Léxico para o compilador TINY-C
 * Implementação da fase de análise léxica
 *
 * Autores:
 * - Marcello Gonzatto Birkan
 * - Daniela Brazolin Flauto
 */

#include "lexico.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Variáveis globais
static FILE *arquivo_fonte;
static int linha_atual = 1;
static int proximo_char;
static char buffer_lexema[256];

// Tabela de palavras reservadas
static struct {
  char *palavra;
  TAtomo atomo;
} palavras_reservadas[] = {
    {"char", CHAR}, {"else", ELSE},   {"if", IF},
    {"int", INT},   {"main", MAIN},   {"readint", READINT},
    {"void", VOID}, {"while", WHILE}, {"writeint", WRITEINT},
    {NULL, ERRO} // Marca o fim da tabela
};

// Nomes dos átomos para mensagens de erro
static const char *nomes_atomos[] = {"ERRO",        "char",
                                     "else",        "if",
                                     "int",         "main",
                                     "readint",     "void",
                                     "while",       "writeint",
                                     "abre_par",    "fecha_par",
                                     "abre_chaves", "fecha_chaves",
                                     "virgula",     "ponto_virgula",
                                     "igual",       "menor",
                                     "menor_igual", "igual_igual",
                                     "diferente",   "maior",
                                     "maior_igual", "or",
                                     "and",         "mais",
                                     "menos",       "vezes",
                                     "divide",      "id",
                                     "intconst",    "charconst",
                                     "comentario",  "fim de arquivo"};

// Inicializa o analisador léxico com o arquivo fonte
void inicializa_lexico(FILE *arquivo) {
  arquivo_fonte = arquivo;
  linha_atual = 1;
  proximo_char = fgetc(arquivo_fonte);
}

// Retorna o nome do átomo como string
const char *atomo_para_string(TAtomo atomo) { return nomes_atomos[atomo]; }

// Função auxiliar para verificar se um lexema é uma palavra reservada
static TAtomo verifica_palavra_reservada(const char *lexema) {
  int i = 0;
  while (palavras_reservadas[i].palavra != NULL) {
    if (strcmp(palavras_reservadas[i].palavra, lexema) == 0) {
      return palavras_reservadas[i].atomo;
    }
    i++;
  }
  return IDENTIFICADOR; // Se não for palavra reservada, é um identificador
}

// Função para obter o próximo átomo do arquivo fonte
TInfoAtomo obter_atomo() {
  TInfoAtomo info;
  int i;

  // Inicializa a estrutura
  info.atomo = ERRO;
  info.linha = linha_atual;
  info.valor_int = 0;
  info.valor_char = '\0';
  info.lexema[0] = '\0';

  // Elimina espaços em branco e controle
  while (proximo_char != EOF &&
         (proximo_char == ' ' || proximo_char == '\t' || proximo_char == '\n' ||
          proximo_char == '\r')) {
    if (proximo_char == '\n') {
      linha_atual++;
    }
    proximo_char = fgetc(arquivo_fonte);
  }

  // Atualiza a linha do átomo após eliminar espaços
  info.linha = linha_atual;

  // Verifica se chegou ao fim do arquivo
  if (proximo_char == EOF) {
    info.atomo = EOS;
    return info;
  }

  // Identificadores e palavras reservadas
  if (isalpha(proximo_char) || proximo_char == '_') {
    i = 0;
    buffer_lexema[i++] = proximo_char;
    proximo_char = fgetc(arquivo_fonte);

    while ((isalnum(proximo_char) || proximo_char == '_') && i < 15) {
      buffer_lexema[i++] = proximo_char;
      proximo_char = fgetc(arquivo_fonte);
    }

    // Verifica se o identificador excede o tamanho máximo
    if (i >= 15 && (isalnum(proximo_char) || proximo_char == '_')) {
      printf("# %d:erro lexico, identificador muito longo\n", linha_atual);
      info.atomo = ERRO;
      // Consome o resto do identificador
      while (isalnum(proximo_char) || proximo_char == '_') {
        proximo_char = fgetc(arquivo_fonte);
      }
      return info;
    }

    buffer_lexema[i] = '\0';

    // Limitando a cópia para 15 caracteres para evitar truncamento
    if (strlen(buffer_lexema) >= sizeof(info.lexema) - 1) {
      // Copia apenas o que cabe no campo lexema, deixando espaço para o
      // terminador nulo
      strncpy(info.lexema, buffer_lexema, sizeof(info.lexema) - 1);
      info.lexema[sizeof(info.lexema) - 1] = '\0';
    } else {
      strcpy(info.lexema, buffer_lexema);
    }

    // Verifica se é palavra reservada ou identificador
    info.atomo = verifica_palavra_reservada(buffer_lexema);
    return info;
  }

  // Constantes inteiras hexadecimais e decimais
  if (isdigit(proximo_char)) {
    i = 0;
    buffer_lexema[i++] = proximo_char;

    // Verifica se começa com 0 (possível hexadecimal)
    if (proximo_char == '0') {
      proximo_char = fgetc(arquivo_fonte);

      // Hexadecimal
      if (proximo_char == 'x' || proximo_char == 'X') {
        buffer_lexema[i++] = proximo_char;
        proximo_char = fgetc(arquivo_fonte);

        // Lê dígitos hexadecimais
        if (isxdigit(proximo_char)) {
          do {
            buffer_lexema[i++] = proximo_char;
            proximo_char = fgetc(arquivo_fonte);
          } while (isxdigit(proximo_char) && i < 255);

          buffer_lexema[i] = '\0';
          info.atomo = INTCONST;

          // Limitando a cópia para 15 caracteres para evitar truncamento
          if (strlen(buffer_lexema) >= sizeof(info.lexema) - 1) {
            // Copia apenas o que cabe no campo lexema, deixando espaço para o
            // terminador nulo
            strncpy(info.lexema, buffer_lexema, sizeof(info.lexema) - 1);
            info.lexema[sizeof(info.lexema) - 1] = '\0';
          } else {
            strcpy(info.lexema, buffer_lexema);
          }

          // Converte string para valor inteiro
          sscanf(buffer_lexema, "%x", &info.valor_int);
          return info;
        } else {
          // Erro: esperava dígito hexadecimal após 0x
          printf("# %d:erro lexico, esperava digito hexadecimal apos 0x\n",
                 linha_atual);
          info.atomo = ERRO;
          return info;
        }
      } else {
        // É um zero decimal
        buffer_lexema[i] = '\0';
        info.atomo = INTCONST;

        // Limitando a cópia para 15 caracteres para evitar truncamento
        if (strlen(buffer_lexema) >= sizeof(info.lexema) - 1) {
          // Copia apenas o que cabe no campo lexema, deixando espaço para o
          // terminador nulo
          strncpy(info.lexema, buffer_lexema, sizeof(info.lexema) - 1);
          info.lexema[sizeof(info.lexema) - 1] = '\0';
        } else {
          strcpy(info.lexema, buffer_lexema);
        }

        info.valor_int = 0;

        // Se não for início de outro token, consome
        if (!isalnum(proximo_char) && proximo_char != '_') {
          return info;
        } else {
          ungetc(proximo_char, arquivo_fonte);
          proximo_char = '0';
          return info;
        }
      }
    } else {
      // Número decimal não começando com 0
      while (isdigit(proximo_char) && i < 255) {
        buffer_lexema[i++] = proximo_char;
        proximo_char = fgetc(arquivo_fonte);
      }

      buffer_lexema[i] = '\0';
      info.atomo = INTCONST;

      // Limitando a cópia para 15 caracteres para evitar truncamento
      if (strlen(buffer_lexema) >= sizeof(info.lexema) - 1) {
        // Copia apenas o que cabe no campo lexema, deixando espaço para o
        // terminador nulo
        strncpy(info.lexema, buffer_lexema, sizeof(info.lexema) - 1);
        info.lexema[sizeof(info.lexema) - 1] = '\0';
      } else {
        strcpy(info.lexema, buffer_lexema);
      }

      info.valor_int = atoi(buffer_lexema);
      return info;
    }
  }

  // Constante caractere
  if (proximo_char == '\'') {
    proximo_char = fgetc(arquivo_fonte);

    if (proximo_char != EOF && proximo_char != '\'') {
      info.valor_char = proximo_char;
      info.atomo = CHARCONST;
      snprintf(info.lexema, sizeof(info.lexema), "'%c'", proximo_char);

      proximo_char = fgetc(arquivo_fonte);

      if (proximo_char == '\'') {
        proximo_char = fgetc(arquivo_fonte);
        return info;
      } else {
        // Erro: esperava fecha aspas simples
        printf("# %d:erro lexico, esperava ' para fechar constante caractere\n",
               linha_atual);
        info.atomo = ERRO;
        return info;
      }
    } else {
      // Erro: constante caractere vazia
      printf("# %d:erro lexico, constante caractere vazia\n", linha_atual);
      info.atomo = ERRO;
      if (proximo_char == '\'') {
        proximo_char = fgetc(arquivo_fonte);
      }
      return info;
    }
  }

  // Comentários e operadores com 2 caracteres
  if (proximo_char == '/') {
    proximo_char = fgetc(arquivo_fonte);

    // Comentário de linha //
    if (proximo_char == '/') {
      i = 0;
      buffer_lexema[i++] = '/';
      buffer_lexema[i++] = '/';

      proximo_char = fgetc(arquivo_fonte);
      while (proximo_char != EOF && proximo_char != '\n' && i < 254) {
        buffer_lexema[i++] = proximo_char;
        proximo_char = fgetc(arquivo_fonte);
      }

      if (proximo_char == '\n') {
        linha_atual++;
        proximo_char = fgetc(arquivo_fonte);
      }

      buffer_lexema[i] = '\0';
      info.atomo = COMENTARIO;
      return info;
    }
    // Comentário de bloco /* */
    else if (proximo_char == '*') {
      i = 0;
      buffer_lexema[i++] = '/';
      buffer_lexema[i++] = '*';

      int linha_inicial = linha_atual;
      proximo_char = fgetc(arquivo_fonte);
      int estado = 0; // 0: dentro do comentário, 1: possível fim

      while (proximo_char != EOF && estado != 2 && i < 254) {
        buffer_lexema[i++] = proximo_char;

        if (estado == 0 && proximo_char == '*') {
          estado = 1;
        } else if (estado == 1 && proximo_char == '/') {
          estado = 2; // Fim do comentário
        } else {
          estado = 0;
        }

        if (proximo_char == '\n') {
          linha_atual++;
        }

        proximo_char = fgetc(arquivo_fonte);
      }

      if (estado == 2) {
        buffer_lexema[i] = '\0';
        info.atomo = COMENTARIO;
        info.linha = linha_inicial; // Mantém a linha inicial do comentário
        return info;
      } else {
        // Erro: comentário não fechado
        printf("# %d:erro lexico, comentario nao fechado\n", linha_atual);
        info.atomo = ERRO;
        return info;
      }
    }
    // Operador de divisão
    else {
      info.atomo = DIVIDE;
      snprintf(info.lexema, sizeof(info.lexema), "/");
      return info;
    }
  }

  // Operadores relacionais e lógicos
  switch (proximo_char) {
  case '<':
    proximo_char = fgetc(arquivo_fonte);
    if (proximo_char == '=') {
      info.atomo = MENOR_IGUAL;
      snprintf(info.lexema, sizeof(info.lexema), "<=");
      proximo_char = fgetc(arquivo_fonte);
    } else {
      info.atomo = MENOR;
      snprintf(info.lexema, sizeof(info.lexema), "<");
    }
    return info;

  case '>':
    proximo_char = fgetc(arquivo_fonte);
    if (proximo_char == '=') {
      info.atomo = MAIOR_IGUAL;
      snprintf(info.lexema, sizeof(info.lexema), ">=");
      proximo_char = fgetc(arquivo_fonte);
    } else {
      info.atomo = MAIOR;
      snprintf(info.lexema, sizeof(info.lexema), ">");
    }
    return info;

  case '=':
    proximo_char = fgetc(arquivo_fonte);
    if (proximo_char == '=') {
      info.atomo = IGUAL_IGUAL;
      snprintf(info.lexema, sizeof(info.lexema), "==");
      proximo_char = fgetc(arquivo_fonte);
    } else {
      info.atomo = IGUAL;
      snprintf(info.lexema, sizeof(info.lexema), "=");
    }
    return info;

  case '!':
    proximo_char = fgetc(arquivo_fonte);
    if (proximo_char == '=') {
      info.atomo = DIFERENTE;
      snprintf(info.lexema, sizeof(info.lexema), "!=");
      proximo_char = fgetc(arquivo_fonte);
    } else {
      // Erro: operador não reconhecido
      printf("# %d:erro lexico, operador nao reconhecido\n", linha_atual);
      info.atomo = ERRO;
    }
    return info;

  case '|':
    proximo_char = fgetc(arquivo_fonte);
    if (proximo_char == '|') {
      info.atomo = OR;
      snprintf(info.lexema, sizeof(info.lexema), "||");
      proximo_char = fgetc(arquivo_fonte);
    } else {
      // Erro: operador não reconhecido
      printf("# %d:erro lexico, operador nao reconhecido\n", linha_atual);
      info.atomo = ERRO;
    }
    return info;

  case '&':
    proximo_char = fgetc(arquivo_fonte);
    if (proximo_char == '&') {
      info.atomo = AND;
      snprintf(info.lexema, sizeof(info.lexema), "&&");
      proximo_char = fgetc(arquivo_fonte);
    } else {
      // Erro: operador não reconhecido
      printf("# %d:erro lexico, operador nao reconhecido\n", linha_atual);
      info.atomo = ERRO;
    }
    return info;
  }

  // Símbolos e operadores simples
  switch (proximo_char) {
  case '(':
    info.atomo = ABRE_PAR;
    snprintf(info.lexema, sizeof(info.lexema), "(");
    break;

  case ')':
    info.atomo = FECHA_PAR;
    snprintf(info.lexema, sizeof(info.lexema), ")");
    break;

  case '{':
    info.atomo = ABRE_CHAVES;
    snprintf(info.lexema, sizeof(info.lexema), "{");
    break;

  case '}':
    info.atomo = FECHA_CHAVES;
    snprintf(info.lexema, sizeof(info.lexema), "}");
    break;

  case ',':
    info.atomo = VIRGULA;
    snprintf(info.lexema, sizeof(info.lexema), ",");
    break;

  case ';':
    info.atomo = PONTO_VIRGULA;
    snprintf(info.lexema, sizeof(info.lexema), ";");
    break;

  case '+':
    info.atomo = MAIS;
    snprintf(info.lexema, sizeof(info.lexema), "+");
    break;

  case '-':
    info.atomo = MENOS;
    snprintf(info.lexema, sizeof(info.lexema), "-");
    break;

  case '*':
    info.atomo = VEZES;
    snprintf(info.lexema, sizeof(info.lexema), "*");
    break;

  default:
    // Caractere não reconhecido
    printf("# %d:erro lexico, caractere nao reconhecido: %c\n", linha_atual,
           proximo_char);
    info.atomo = ERRO;
    break;
  }

  // Avança para o próximo caractere
  if (info.atomo != ERRO && info.atomo != EOS) {
    proximo_char = fgetc(arquivo_fonte);
  }

  return info;
}