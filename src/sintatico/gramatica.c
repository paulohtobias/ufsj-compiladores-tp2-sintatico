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
int pcc_gramatica_g_print_gramatica = 0;

void pcc_gramatica_init(const char *pcc_gramatica_std) {
	lexico_init();

	if (pcc_gramatica_std == NULL) {
		pcc_gramatica_std = pcc_gramatica_g_std;
	}
	pcc_ll1_de_arquivo(&gramatica_lexico, pcc_gramatica_std);

	if (pcc_gramatica_g_print_gramatica > 0) {
		pcc_ll1_print(&gramatica_lexico);

		if (pcc_gramatica_g_print_gramatica > 1) {
			pcc_gramatica_finalizar();
			pcc_codigo_fonte_finalizar();
			exit(0);
		}
	}
}

void pcc_gramatica_finalizar() {
	lexico_finalizar();

	pcc_ll1_liberar(&gramatica_lexico);
}

int32_t *pcc_gramatica_analisar(const char *nome_arquivo) {
	lexico_parse(nome_arquivo);

	return pcc_ll1_reconhecer(&gramatica_lexico, lista_tokens);
}
