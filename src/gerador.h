/*
 * Gerador de Código MEPA para o compilador TINY-C
 * Implementação da fase de geração de código intermediário
 *
 * Autores:
 * - Marcello Gonzatto Birkan
 * - Daniela Brazolin Flauto
 */

#ifndef _GERADOR_H
#define _GERADOR_H

// Inicializa o gerador de código
void inicializa_gerador();

// Função para gerar o próximo rótulo sequencial (L1, L2, etc.)
int proximo_rotulo();

// Gera código de inicialização do programa
void gera_inicio_programa();

// Gera código de finalização do programa
void gera_fim_programa();

// Gera código para alocar memória para variáveis
void gera_alocacao_memoria(int num_variaveis);

// Gera código para acesso a variável
void gera_carregar_valor(int endereco);        // CRVL
void gera_armazenar_valor(int endereco);       // ARMZ
void gera_carregar_constante(int valor);       // CRCT

// Gera código para operações aritméticas
void gera_soma();                             // SOMA
void gera_subtracao();                        // SUBT
void gera_multiplicacao();                    // MULT
void gera_divisao();                          // DIVI

// Gera código para operações relacionais
void gera_igual();                            // CMIG
void gera_diferente();                        // CMDG
void gera_menor();                            // CMME
void gera_menor_igual();                      // CMEG
void gera_maior();                            // CMMA
void gera_maior_igual();                      // CMAG

// Gera código para operações lógicas
void gera_ou();                               // DISJ
void gera_e();                                // CONJ

// Gera código para desvios condicionais e incondicionais
void gera_desvio_incondicional(int rotulo);   // DSVS
void gera_desvio_se_falso(int rotulo);        // DSVF
void gera_nada(int rotulo);                   // Label: NADA

// Gera código para entrada e saída
void gera_entrada();                          // LEIT
void gera_saida();                            // IMPR

#endif // _GERADOR_H