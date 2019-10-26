/**
 * PCC - Paulo C Compiler
 *
 * gramatica.h
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#ifndef GRAMATICA_H
#define GRAMATICA_H

#include "sintatico/ll1.h"

#ifndef PCC_GRAMATICA_ARQUIVO
#define PCC_GRAMATICA_ARQUIVO "gramática.pccg"
#endif // PCC_GRAMATICA_ARQUIVO

void pcc_gramatica_init();

void pcc_gramatica_finalizar();

void pcc_gramatica_teste(const char *nome_arquivo);

#endif // GRAMATICA_H
