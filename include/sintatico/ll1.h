/**
 * PCC - Paulo C Compiler
 *
 * ll1.h
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#ifndef LL1_H
#define LL1_H

#include <stdbool.h>
#include <stdint.h>
#include "lexico/token.h"

/// Controla o nível das mensagens de debug. 0: desligado.
extern int pcc_ll1_g_debug;

enum {
	SIMBOLO_TERMINAL,
	SIMBOLO_VARIAVEL
};

typedef struct pcc_simbolo_id_terminal_t {
	int32_t tipo;
	int32_t subtipo;
} pcc_simbolo_id_terminal_t;

typedef struct pcc_variavel_t {
	int32_t cod;
	char *nome;

	bool gera_vazio;
	pcc_simbolo_id_terminal_t *first;
	pcc_simbolo_id_terminal_t *follow;

	struct pcc_ll1_M_t {
		pcc_simbolo_id_terminal_t token;
		int32_t producao_id;
	} *M;
} pcc_variavel_t;

typedef union pcc_simbolo_id_u {
	pcc_simbolo_id_terminal_t token;

	int32_t variavel;
} pcc_simbolo_id_u;

typedef struct pcc_simbolo_t {
	int8_t tipo;

	pcc_simbolo_id_u id;
} pcc_simbolo_t;

typedef struct pcc_producao_t {
	/// Índice da produção na lista de produções da gramática.
	int32_t id;

	/// Id da variável.
	int32_t origem;

	/// Cadeia de símbolos (terminais ou não terminais) que
	/// substituirão `origem`.
	pcc_simbolo_t *simbolos;

	/// Função a ser executada quando a produção for usada.
	void (*acao)(void *);
} pcc_producao_t;

typedef struct pcc_ll1_t {
	/// Lista de variáveis.
	pcc_variavel_t *variaveis;

	/// Variável que inicia a análise.
	int32_t variavel_inicial;

	/// Lista com todas as produções da gramática.
	pcc_producao_t *producoes;
} pcc_ll1_t;

void pcc_ll1_criar(pcc_ll1_t *gramatica, char **nome_variaveis);

void pcc_ll1_liberar(pcc_ll1_t *gramatica);

void pcc_ll1_add_producao(pcc_ll1_t *gramatica, pcc_producao_t producao);

void pcc_ll1_calcular(pcc_ll1_t *gramatica);

void pcc_ll1_de_arquivo(pcc_ll1_t *gramatica, const char *padrao);

int32_t *pcc_ll1_reconhecer(pcc_ll1_t *gramatica, token_t *lista_tokens);

void pcc_ll1_simbolo_print(const pcc_ll1_t *gramatica, const pcc_simbolo_t *simbolo);

void pcc_ll1_producao_print(const pcc_ll1_t *gramatica, const pcc_producao_t *producao);

void pcc_ll1_print(const pcc_ll1_t *gramatica);

void pcc_ll1_gramatica_cabecalho();

#endif // LL1_H
