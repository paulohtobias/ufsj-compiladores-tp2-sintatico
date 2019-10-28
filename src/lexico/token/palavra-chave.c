/**
 * PCC - Paulo C Compiler
 *
 * token/palavra-chave.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <string.h>
#include "lexico/token/palavra-chave.h"
#include "utils.h"
#include "plist.h"

/// Tipos de palavra-chave em string.
#define TK_KW_SUBTIPO(cod, str) str,
const char __palavras_chave[][16] = {
	TK_KW_SUBTIPOS
};
#undef TK_KW_SUBTIPO
size_t __palavras_chave_quantidade = ARR_TAMANHO(__palavras_chave);

int token_palavra_chave_init(afd_t *afd) {
	// Inicializando a tabela de símbolos de palavra-chave.
	plist_create(tabela_simbolos[TK_KW], __palavras_chave_quantidade);
	memset(tabela_simbolos[TK_KW], 0, __palavras_chave_quantidade * sizeof *tabela_simbolos[TK_KW]);

	// Inicializando a matriz de descrição.
	#define TK_KW_SUBTIPO(cod, str) \
		plist_append(__token_str[TK_KW], str);

	TK_KW_SUBTIPOS
	#undef TK_KW_SUBTIPO

	return 0;
}

int token_palavra_chave_buscar(const token_t *token) {
	int inicio = 0;
	int fim = __palavras_chave_quantidade;
	int index = -1;

	while (1) {
		int novo_index = (int) ((inicio + fim) / 2);
		if (novo_index == index) {
			return -1;
		}

		index = novo_index;

		int i;
		for (i = 0; __palavras_chave[index][i] != '\0' && i < token->contexto.lexema_comprimento; i++) {
			if (token->contexto._lexema[i] < __palavras_chave[index][i]) {
				fim = index;
				break;
			} else if (token->contexto._lexema[i] > __palavras_chave[index][i]) {
				inicio = index;
				break;
			}
		}

		if (__palavras_chave[index][i] == '\0' && i == token->contexto.lexema_comprimento) {
			return index;
		}
	}
}

const char *token_palavra_chave_subtipo_str(int32_t subtipo) {
	if (subtipo < __palavras_chave_quantidade) {
		return __palavras_chave[subtipo];
	}

	return "Erro: subtipo de palavra-chave inválido";
}
