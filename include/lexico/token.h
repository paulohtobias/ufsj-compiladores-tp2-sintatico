/**
 * PCC - Paulo C Compiler
 *
 * token.h
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "lexico/afd.h"
#include "log.h"
#include "lexico/codigo_fonte.h"

// X-macro para gerar os códigos de token.
#define TOKEN_CODIGOS \
	TOKEN_CODIGO(TK_ID, identificador, "Identificador") \
	TOKEN_CODIGO(TK_KW, palavra_chave, "Palavra-chave") \
	TOKEN_CODIGO(TK_CNST, constante, "Constante") \
	TOKEN_CODIGO(TK_OP, operador, "Operador") \
	TOKEN_CODIGO(TK_EXT, extra, "Extra") \
	TOKEN_CODIGO(TK_CMT, comentario, "Comentário") \
	TOKEN_CODIGO(TK_PP, preprocessador, "Pré-processador")


/// Tipos básicos de token.
#define TOKEN_CODIGO(cod, nome, descricao) cod,
enum {
	TOKEN_CODIGOS

	TK_COUNT // Quantidade de tipos de token.
};
#undef TOKEN_CODIGO
#define TK_$ -1

extern char **__token_str[TK_COUNT];

/// Funções init.
#define TOKEN_CODIGO(cod, nome, descricao) int token_ ## nome ## _init(afd_t *afd);
TOKEN_CODIGOS
#undef TOKEN_CODIGO

typedef struct token_contexto_t {
	const pcc_codigo_fonte_t *fonte;

	/// Aponta pra linha_src + posicao.coluna
	char * _lexema;
	size_t lexema_comprimento;

	struct {
		int32_t linha;
		int32_t coluna;
	} posicao;
} token_contexto_t;

typedef struct token_t {
	int32_t tipo;
	int32_t subtipo;

	token_contexto_t contexto;

	struct {
		void *dados;
		size_t tamanho;

		char *(*to_str)(const void *, size_t);
	} valor;

	const char *(*subtipo_to_str)(int32_t);
} token_t;


/// Lista de tokens.
extern token_t *lista_tokens;

/**
 * Tabela de símbolos. Armazena o índice do token na
 * lista de tokens para evitar redundância e reduzir
 * o gasto de memória.
 */
size_t **tabela_simbolos[TK_COUNT];


/**
 * Cria o autômato léxico para todos os tipos e sub-tipos
 * de token existente.
 */
int token_init(afd_t *afd);

/**
 * Libera memória de todos os tokens na lista de tokens
 * e tabela de símbolos.
 */
void token_finalizar();

/**
 * Retorna a string correspondente ao tipo do token.
 * Se o tipo for inválido, retorna uma string vazia.
 */
const char *token_tipo_str(const token_t *token);

/**
 * Retorna a string correspondente ao subtipo do token.
 * Se o subtipo for inválido, retorna uma string vazia.
 */
const char *token_subtipo_str(const token_t *token);

/**
 * Retorna uma string correspondente ao tipo e subtipo
 * do token sem a necessidade de se ter um token.
 */
const char *token_tipo_subtipo_str(int32_t tipo, int32_t subtipo);

/**
 * Retorna o tipo e o subtipo de um token a partir de
 * uma string.
 */
bool token_str_tipo_subtipo(const char *str, int32_t *tipo, int32_t *subtipo);

/**
 * Função genérica para criar um novo token.
 */
token_t token_criar(int32_t tipo, int32_t subtipo, const void *contexto);

/**
 * Libera os dados do token que foram alocado dinâmicamente.
 * Se o próprio token foi alocado dinâmicamente, este não será
 * liberado.
 */
void token_liberar(token_t *token);

/**
 * Adiciona o token na lista de tokens bem como na
 * tabela de símbolos correspondente.
 */
void token_adicionar(const token_t *token);

/**
 * TODO
 */
void token_print(FILE *out, const token_t *token);

#endif // TOKEN_H
