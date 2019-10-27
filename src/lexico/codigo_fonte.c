/**
 * PCC - Paulo C Compiler
 *
 * lexico/codigo_fonte.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <string.h>
#include "plist.h"
#include "pdict.h"
#include "log.h"
#include "lexico/codigo_fonte.h"

pdict_t *fontes = NULL;

void pcc_codigo_fonte_init() {
	fontes = pdict_create();
}

void pcc_codigo_fonte_finalizar() {
	pdict_destroy(fontes);
}

pcc_codigo_fonte *pcc_codigo_fonte_abir(const char *nome_arquivo) {
	pdict_item_t *item = pdict_get_item(fontes, nome_arquivo);
	pcc_codigo_fonte *fonte;
	if (item == NULL) {
		PMALLOC(fonte, 1);

		fonte->src = file_to_str(nome_arquivo, &fonte->src_tamanho);
		if (fonte->src == NULL) {
			LOG_PCC_ERRO(1, "", "Não foi possível abrir o arquivo '%s': ", nome_arquivo);
		}

		fonte->caminho = strdup(nome_arquivo);
		fonte->linhas_qtd = 0;
		fonte->offset_linhas = NULL;

		pdict_add_value_all(fontes, nome_arquivo, fonte, (void (*)(void *)) pcc_codigo_fonte_liberar);
	} else {
		fonte = item->value;
	}

	return fonte;
}

char *pcc_codigo_fonte_get_linha(pcc_codigo_fonte *fonte, int32_t linha, int32_t coluna) {
	// Decremento pois linha e coluna começam de 1.
	linha--;
	coluna--;

	// Coluna negativa significa pegar a linha toda.
	if (coluna < 0) {
		coluna = 0;
	}

	return fonte->src + (fonte->offset_linhas[linha - 1] + coluna);
}

void pcc_codigo_fonte_liberar(pcc_codigo_fonte *fonte) {
	free(fonte->caminho);
	free(fonte->src);
	plist_free(fonte->offset_linhas);
	free(fonte);
}
