/*
 * Analisador Semântico para o compilador TINY-C
 * Implementação da fase de análise semântica
 *
 * Autores:
 * - Marcello Gonzatto Birkan
 * - Daniela Brazolin Flauto
 */

#ifndef _SEMANTICO_H
#define _SEMANTICO_H

// Estrutura para implementação da tabela de símbolos como lista encadeada
typedef struct _TNo {
    char ID[16];
    int endereco;
    struct _TNo *prox;
} TNo;

// Inicializa o analisador semântico
void inicializa_semantico();

// Função para inserir uma variável na tabela de símbolos
// Retorna: 1 se inseriu com sucesso, 0 se já existe (erro semântico)
int insere_tabela_simbolos(const char *id);

// Função para buscar uma variável na tabela de símbolos
// Retorna: o endereço da variável ou -1 se não encontrada (erro semântico)
int busca_tabela_simbolos(const char *id);

// Função para exibir o conteúdo da tabela de símbolos
void imprime_tabela_simbolos();

// Função para liberar a memória alocada pela tabela de símbolos
void finaliza_semantico();

#endif // _SEMANTICO_H