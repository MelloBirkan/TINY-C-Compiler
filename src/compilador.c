/*
 * Compilador para a linguagem TINY-C
 * Implementação das fases de análise léxica, sintática, semântica e geração de código
 *
 * Autores:
 * - Marcello Gonzatto Birkan
 * - Daniela Brazolin Flauto
 */

#include "lexico.h"
#include "sintatico.h"
#include "semantico.h"
#include "gerador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  FILE *arquivo_fonte;

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
  inicializa_semantico();
  inicializa_gerador();
  inicializa_sintatico();

  // Inicia a análise sintática e geração de código
  program();

  // Verifica se chegou ao fim do arquivo
  consome(EOS);

  // Finaliza os analisadores
  finaliza_semantico();

  // Fecha o arquivo fonte
  fclose(arquivo_fonte);

  return 0;
}