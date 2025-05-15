/*
 * Gerador de Código MEPA para o compilador TINY-C
 * Implementação da fase de geração de código intermediário
 *
 * Autores:
 * - Marcello Gonzatto Birkan
 * - Daniela Brazolin Flauto
 */

#include "gerador.h"
#include <stdio.h>

// Variáveis globais
static int contador_rotulos = 0;

// Inicializa o gerador de código
void inicializa_gerador() {
    contador_rotulos = 0;
}

// Retorna o próximo rótulo sequencial (L1, L2, etc.)
int proximo_rotulo() {
    return ++contador_rotulos;
}

// Gera código de inicialização do programa
void gera_inicio_programa() {
    printf("INPP\n");
}

// Gera código de finalização do programa
void gera_fim_programa() {
    printf("PARA\n");
}

// Gera código para alocar memória para variáveis
void gera_alocacao_memoria(int num_variaveis) {
    printf("AMEM %d\n", num_variaveis);
}

// Gera código para acesso a variável
void gera_carregar_valor(int endereco) {
    printf("CRVL %d\n", endereco);
}

void gera_armazenar_valor(int endereco) {
    printf("ARMZ %d\n", endereco);
}

void gera_carregar_constante(int valor) {
    printf("CRCT %d\n", valor);
}

// Gera código para operações aritméticas
void gera_soma() {
    printf("SOMA\n");
}

void gera_subtracao() {
    printf("SUBT\n");
}

void gera_multiplicacao() {
    printf("MULT\n");
}

void gera_divisao() {
    printf("DIVI\n");
}

// Gera código para operações relacionais
void gera_igual() {
    printf("CMIG\n");
}

void gera_diferente() {
    printf("CMDG\n");
}

void gera_menor() {
    printf("CMME\n");
}

void gera_menor_igual() {
    printf("CMEG\n");
}

void gera_maior() {
    printf("CMMA\n");
}

void gera_maior_igual() {
    printf("CMAG\n");
}

// Gera código para operações lógicas
void gera_ou() {
    printf("DISJ\n");
}

void gera_e() {
    printf("CONJ\n");
}

// Gera código para desvios condicionais e incondicionais
void gera_desvio_incondicional(int rotulo) {
    printf("DSVS L%d\n", rotulo);
}

void gera_desvio_se_falso(int rotulo) {
    printf("DSVF L%d\n", rotulo);
}

void gera_nada(int rotulo) {
    printf("L%d:\tNADA\n", rotulo);
}

// Gera código para entrada e saída
void gera_entrada() {
    printf("LEIT\n");
}

void gera_saida() {
    printf("IMPR\n");
}