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

const char *pcc_gramatica_g_std = PCC_GRAMATICA_ARQUIVO;
bool pcc_gramatica_g_print_gramatica = true;

void pcc_gramatica_init(const char *pcc_gramatica_std) {
	// TODO
	lexico_init();

	if (pcc_gramatica_std == NULL) {
		pcc_gramatica_std = pcc_gramatica_g_std;
	}
	pcc_ll1_de_arquivo(&gramatica_lexico, pcc_gramatica_std);

	if (pcc_gramatica_g_print_gramatica) {
		pcc_ll1_print(&gramatica_lexico);

		pcc_gramatica_finalizar();
		pcc_codigo_fonte_finalizar();
		exit(0);
	}
}

void pcc_gramatica_finalizar() {
	/// TODO:
	lexico_finalizar();

	pcc_ll1_liberar(&gramatica_lexico);
}

void pcc_gramatica_teste(const char *nome_arquivo) {
	lexico_parse(nome_arquivo);

	/**
	for (size_t i = 0; i < plist_len(lista_tokens); i++) {
		token_print(stdout, lista_tokens + i);
	}
	/**/
	//exit(0);


	pcc_ll1_reconhecer(&gramatica_lexico, lista_tokens);
}
