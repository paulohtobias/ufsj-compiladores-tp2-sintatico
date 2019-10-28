/**
 * PCC - Paulo C Compiler
 *
 * token.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <string.h>
#include "lexico/token.h"
#include "utils.h"
#include "plist.h"

/// Descrição dos códigos.
#define TOKEN_CODIGO(cod, nome, descricao) descricao,
const char __token_codigo_str[][64] = {
	TOKEN_CODIGOS
};
#undef TOKEN_CODIGO

/**
 * "Matriz(i, j, elem) de strings" onde
 * i: tipo do token
 * j: subtipo do token
 * elem: representação em string do token
 */
char **__token_str[TK_COUNT] = {0};


/// Lista de tokens.
token_t *lista_tokens = NULL;

/// Tabela de símbolos.
size_t **tabela_simbolos[TK_COUNT];


int token_init(afd_t *afd) {
	#define TOKEN_CODIGO(cod, nome, descricao) token_ ## nome ## _init(afd);
	TOKEN_CODIGOS
	#undef TOKEN_CODIGO

	return 0;
}

void token_finalizar() {
	for (size_t i = 0; i < plist_len(lista_tokens); i++) {
		token_liberar(lista_tokens + i);
	}
	plist_free(lista_tokens);

	for (size_t i = 0; i < TK_COUNT; i++) {
		for (int j = 0; j < plist_cap(tabela_simbolos[i]); j++) {
			plist_free(tabela_simbolos[i][j]);
		}
		plist_free(tabela_simbolos[i]);
		plist_free(__token_str[i]);
	}
}

const char *token_tipo_str(const token_t *token) {
	if (token->tipo < TK_COUNT) {
		return __token_codigo_str[token->tipo];
	}

	return "";
}

const char *token_subtipo_str(const token_t *token) {
	if (token->subtipo_to_str != NULL) {
		return token->subtipo_to_str(token->subtipo);
	}

	return "";
}

const char *token_tipo_subtipo_str(uint32_t tipo, uint32_t subtipo) {
	if (tipo >= TK_COUNT || subtipo >= plist_len(__token_str[tipo])) {
		return "";
	}

	return __token_str[tipo][subtipo];
}

bool token_str_tipo_subtipo(const char *str, uint32_t *tipo, uint32_t *subtipo) {
	for (*tipo = 0; *tipo < TK_COUNT; (*tipo)++) {
		for (*subtipo = 0; *subtipo < plist_len(__token_str[*tipo]); (*subtipo)++) {
			if (strcmp(str, __token_str[*tipo][*subtipo]) == 0) {
				return true;
			}
		}
	}

	return false;
}

token_t token_criar(uint32_t tipo, uint32_t subtipo, const void *_contexto) {
	token_t token;

	const token_contexto_t *contexto = _contexto;
	// Tipo e sub-tipo.
	token.tipo = tipo;
	token.subtipo = subtipo;
	token.subtipo_to_str = NULL;

	// Arquivo.
	token.contexto.fonte = contexto->fonte;

	// Posição.
	token.contexto.posicao.linha = contexto->posicao.linha;
	token.contexto.posicao.coluna = contexto->posicao.coluna;

	// Lexema
	token.contexto.lexema_comprimento = contexto->lexema_comprimento;
	token.contexto._lexema = pcc_codigo_fonte_get_linha(token.contexto.fonte, token.contexto.posicao.linha, token.contexto.posicao.coluna);

	// Valor.
	token.valor.dados = NULL;
	token.valor.tamanho = 0;
	token.valor.to_str = NULL;

	return token;
}

void token_liberar(token_t *token) {
	free(token->valor.dados);
}

void token_adicionar(const token_t *token) {
	// Adiciona na lista de tokens.
	plist_append(lista_tokens, *token);

	// Adiciona na tabela de símbolos correspondente.
	plist_append(tabela_simbolos[token->tipo][token->subtipo], plist_len(lista_tokens) - 1);
}

void token_print(FILE *out, const token_t *token) {
	const char *subtipo = token_subtipo_str(token);
	if (*subtipo == '\0') {
		subtipo = "N/A";
	}

	char *_valor = NULL;
	if (token->valor.to_str != NULL) {
		_valor = token->valor.to_str(token->valor.dados, token->valor.tamanho);
	}

	fprintf(out,
		"TOKEN: %s\n"
		"\tSubtipo: %s\n"
		"\tArquivo: %s\n"
		"\tLinha: %u | Coluna: %u\n"
		"\tLexema: %.*s\n",
		__token_codigo_str[token->tipo],
		subtipo,
		token->contexto.fonte->caminho,
		token->contexto.posicao.linha, token->contexto.posicao.coluna,
		(int) token->contexto.lexema_comprimento, token->contexto._lexema
	);
	if (_valor != NULL) {
		fprintf(out, "\tValor: %s\n\n", _valor);
	} else {
		fprintf(out, "\n");
	}

	free(_valor);
}
