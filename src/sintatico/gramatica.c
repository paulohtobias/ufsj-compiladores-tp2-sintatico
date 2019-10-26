/**
 * PCC - Paulo C Compiler
 *
 * gramatica.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <stdlib.h>
#include "plist.h"
#include "lexico/lexico.h"
#include "sintatico/gramatica.h"

pcc_ll1_t gramatica_lexico;

void pcc_gramatica_init() {
	// TODO
	lexico_init();

	pcc_ll1_de_arquivo(&gramatica_lexico, PCC_GRAMATICA_ARQUIVO);
}

void pcc_gramatica_finalizar() {
	/// TODO:
	lexico_finalizar();
}

void pcc_gramatica_teste(const char *nome_arquivo) {
	lexico_parse(nome_arquivo);

	pcc_ll1_reconhecer(&gramatica_lexico, lista_tokens);
}
