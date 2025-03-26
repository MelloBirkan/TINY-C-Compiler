/*
 * Compilador para a linguagem TINY-C
 * Implementação das fases de análise léxica e sintática
 *
 * Autores:
 * - Marcello Gonzatto Birkan
 * - Daniela Brazolin Flauto
 */

#include "lexico.h"
#include "sintatico.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  FILE *arquivo_fonte;
  int total_linhas = 0;

  // Verifica se foi informado o nome do arquivo fonte
  if (argc != 2) {
    printf("Uso: %s <arquivo_fonte>\n", argv[0]);
    return 1;
  }

  // Abre o arquivo fonte
  arquivo_fonte = fopen(argv[1], "r");
  if (arquivo_fonte == NULL) {
    printf("Erro ao abrir o arquivo %s\n", argv[1]);
    return 1;
  }

  // Inicializa os analisadores
  inicializa_lexico(arquivo_fonte);
  inicializa_sintatico();

  // Inicia a análise sintática
  program();

  // Verifica se chegou ao fim do arquivo
  consome(EOS);

  // Conta o número de linhas do arquivo
  rewind(arquivo_fonte);
  char c;
  total_linhas = 0; // Inicializa contador
  while ((c = fgetc(arquivo_fonte)) != EOF) {
    if (c == '\n') {
      total_linhas++;
    }
  }
  total_linhas++; // Incrementa 1 para contar a última linha que pode não ter \n

  // Informa o total de linhas analisadas
  printf("%d linhas analisadas, programa sintaticamente correto\n",
         total_linhas);

  // Fecha o arquivo fonte
  fclose(arquivo_fonte);

  return 0;
}