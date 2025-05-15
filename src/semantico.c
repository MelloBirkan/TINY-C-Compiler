/*
 * Analisador Semântico para o compilador TINY-C
 * Implementação da fase de análise semântica
 *
 * Autores:
 * - Marcello Gonzatto Birkan
 * - Daniela Brazolin Flauto
 */

#include "semantico.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Variáveis globais
static TNo *tabela_simbolos = NULL;  // Início da lista encadeada
static int proximo_endereco = 0;     // Próximo endereço disponível

// Inicializa o analisador semântico
void inicializa_semantico() {
    // Zera a tabela de símbolos e o contador de endereços
    tabela_simbolos = NULL;
    proximo_endereco = 0;
}

// Insere uma variável na tabela de símbolos
int insere_tabela_simbolos(const char *id) {
    TNo *novo, *atual, *anterior;
    
    // Verifica se o identificador já existe na tabela
    if (busca_tabela_simbolos(id) != -1) {
        return 0; // Erro: variável já declarada
    }
    
    // Aloca memória para o novo nó
    novo = (TNo *)malloc(sizeof(TNo));
    if (novo == NULL) {
        printf("Erro de alocação de memória\n");
        exit(1);
    }
    
    // Preenche os dados do novo nó
    strncpy(novo->ID, id, sizeof(novo->ID) - 1);
    novo->ID[sizeof(novo->ID) - 1] = '\0'; // Garante que a string termine com '\0'
    novo->endereco = proximo_endereco++;
    novo->prox = NULL;
    
    // Caso a lista esteja vazia
    if (tabela_simbolos == NULL) {
        tabela_simbolos = novo;
        return 1;
    }
    
    // Insere o nó no final da lista
    atual = tabela_simbolos;
    while (atual != NULL) {
        anterior = atual;
        atual = atual->prox;
    }
    anterior->prox = novo;
    
    return 1;
}

// Busca uma variável na tabela de símbolos
int busca_tabela_simbolos(const char *id) {
    TNo *atual = tabela_simbolos;
    
    while (atual != NULL) {
        if (strcmp(atual->ID, id) == 0) {
            return atual->endereco; // Retorna o endereço se encontrar
        }
        atual = atual->prox;
    }
    
    return -1; // Não encontrou
}

// Imprime o conteúdo da tabela de símbolos
void imprime_tabela_simbolos() {
    TNo *atual = tabela_simbolos;
    
    printf("TABELA DE SIMBOLOS\n");
    while (atual != NULL) {
        printf("%s | Endereco: %d\n", atual->ID, atual->endereco);
        atual = atual->prox;
    }
}

// Libera a memória alocada pela tabela de símbolos
void finaliza_semantico() {
    TNo *atual = tabela_simbolos;
    TNo *proximo;
    
    while (atual != NULL) {
        proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
    
    tabela_simbolos = NULL;
    proximo_endereco = 0;
}