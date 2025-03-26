/*
 * Analisador Sintático para o compilador TINY-C
 * Implementação da fase de análise sintática
 *
 * Autores:
 * - Marcello Gonzatto Birkan
 * - Daniela Brazolin Flauto
 */

#ifndef _SINTATICO_H
#define _SINTATICO_H

#include "lexico.h"

// Função para inicializar o analisador sintático
void inicializa_sintatico();

// Funções para análise sintática de acordo com a gramática TINY-C
void program();
void compound_stmt();
void var_decl();
void type_specifier();
void var_decl_list();
void variable_id();
void stmt();
void assig_stmt();
void cond_stmt();
void while_stmt();
void expr();
void conjunction();
void comparison();
void relation();
void sum();
void term();
void factor();

// Função para consumir o átomo atual e solicitar o próximo
void consome(TAtomo atomo_esperado);

#endif // _SINTATICO_H 