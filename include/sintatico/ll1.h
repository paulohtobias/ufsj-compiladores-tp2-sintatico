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

/**
 * TODO: mudar a lógica do id da variável
 * pra usar x-macro e deixar os ids pré-definidos.
 * Desse jeito não vai precisar usar string em lugar nenhum.
 */

enum {
	SIMBOLO_TERMINAL,
	SIMBOLO_VARIAVEL
};

typedef struct pcc_simbolo_id_terminal_t {
	uint32_t tipo;
	uint32_t subtipo;
} pcc_simbolo_id_terminal_t;

typedef struct pcc_variavel_t {
	uint32_t cod;
	bool gera_vazio;
	pcc_simbolo_id_terminal_t *first;
	pcc_simbolo_id_terminal_t *follow;

	struct pcc_ll1_M_t {
		pcc_simbolo_id_terminal_t token;
		uint32_t producao_id;
	} *M;
} pcc_variavel_t;

typedef union pcc_simbolo_id_u {
	pcc_simbolo_id_terminal_t token;

	uint32_t variavel;
} pcc_simbolo_id_u;

typedef struct pcc_simbolo_t {
	int8_t tipo;

	pcc_simbolo_id_u id;
} pcc_simbolo_t;

typedef struct pcc_producao_t {
	/// Índice da produção na lista de produções da gramática.
	uint32_t id;

	/// Id da variável.
	uint32_t origem;

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

void pcc_ll1_init(pcc_ll1_t *gramatica, uint16_t qtd_variaveis);

void pcc_ll1_add_producao(pcc_ll1_t *gramatica, pcc_producao_t producao);

void pcc_ll1_calcular(pcc_ll1_t *gramatica);

void pcc_ll1_print(const pcc_ll1_t *gramatica, const char *_variaveis_str, size_t variavel_str_tam);

#endif // LL1_H
