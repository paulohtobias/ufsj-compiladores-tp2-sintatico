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

pcc_ll1_t gramatica;

void pcc_gramatica_init() {
	// TODO
	lexico_init();
}

void pcc_gramatica_teste(const char *nome_arquivo) {
	pcc_ll1_de_arquivo(&gramatica, "gramatica.pccg");

	lexico_parse(nome_arquivo);

	pcc_ll1_reconhecer(&gramatica, lista_tokens);

	lexico_finalizar();
}
