/**
 * PCC - Paulo C Compiler
 *
 * ll1.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <stdlib.h>
#include <string.h>
#include "plist.h"
#include "sintatico/ll1.h"

static void calcular_vazio(pcc_ll1_t *gramatica);
static void calcular_first(pcc_ll1_t *gramatica);
static void calcular_follow(pcc_ll1_t *gramatica);

void pcc_ll1_init(pcc_ll1_t *gramatica, uint16_t qtd_variaveis) {
	plist_create(gramatica->variaveis, qtd_variaveis);
	for (uint16_t i = 0; i < plist_cap(gramatica->variaveis); i++) {
		pcc_variavel_t variavel;
		variavel.cod = i;
		variavel.gera_vazio = false;
		variavel.first = NULL;
		variavel.follow = NULL;
		plist_put(gramatica->variaveis, variavel, i);
	}

	gramatica->variavel_inicial = -1;

	gramatica->producoes = NULL;
}

void pcc_ll1_add_producao(pcc_ll1_t *gramatica, pcc_producao_t producao) {
	// Adiciona a produção na lista de produções.
	producao.id = plist_len(gramatica->producoes);
	if (producao.id == 0) {
		gramatica->variavel_inicial = producao.origem;
	}
	plist_append(gramatica->producoes, producao);
}

void pcc_ll1_calcular(pcc_ll1_t *gramatica) {
	calcular_vazio(gramatica);
	calcular_first(gramatica);
	calcular_follow(gramatica);
}

static void calcular_vazio(pcc_ll1_t *gramatica) {
	// Calculando os óbvios.
	for (size_t i = 0; i < plist_len(gramatica->producoes); i++) {
		const pcc_producao_t *producao = gramatica->producoes + i;

		if (plist_len(producao->simbolos) == 0) {
			gramatica->variaveis[producao->origem].gera_vazio = true;
		}
	}

	bool alteracao;
	do {
		alteracao = false;

		for (size_t i = 0; i < plist_len(gramatica->producoes); i++) {
			const pcc_producao_t *producao = gramatica->producoes + i;

			if (!gramatica->variaveis[producao->origem].gera_vazio) {
				size_t j;
				for (j = 0; j < plist_len(producao->simbolos); j++) {
					const pcc_simbolo_t *simbolo = producao->simbolos + j;

					if (simbolo->tipo == SIMBOLO_TERMINAL || !gramatica->variaveis[simbolo->id.variavel].gera_vazio) {
						break;
					}
				}

				if (j == plist_len(producao->simbolos)) {
					gramatica->variaveis[producao->origem].gera_vazio = true;
					alteracao = true;
				}
			}
		}
	} while (alteracao);
}

static void calcular_first(pcc_ll1_t *gramatica) {
	// Calculando os óbvios.
	for (size_t i = 0; i < plist_len(gramatica->producoes); i++) {
		const pcc_producao_t *producao = gramatica->producoes + i;

		if (plist_len(producao->simbolos) > 0 && producao->simbolos[0].tipo == SIMBOLO_TERMINAL) {
			plist_append(gramatica->variaveis[producao->origem].first, producao->simbolos[0].id.token);
		}
	}

	for (size_t i = 0; i < plist_len(gramatica->producoes); i++) {
		const pcc_producao_t *producao = gramatica->producoes + i;

		for (size_t j = 0; j < plist_len(producao->simbolos); j++) {
			const pcc_simbolo_t *simbolo = producao->simbolos + j;

			if (simbolo->tipo == SIMBOLO_TERMINAL) {
				/*
				 * Se o símbolo for um terminal, adicione-o à lista e
				 * passe para a próxima produção.
				 */

				if (_plist_find(gramatica->variaveis[producao->origem].first, &simbolo->id.token, NULL) == -1) {
					plist_append(gramatica->variaveis[producao->origem].first, simbolo->id.token);
				}

				break;
			} else {
				/*
				 * Se for um não-terminal, então adicione FIRST
				 * do símbolo à lista.
				 */

				for (int32_t k = 0; k < plist_len(gramatica->variaveis[simbolo->id.variavel].first); k++) {
					if (_plist_find(gramatica->variaveis[producao->origem].first, &gramatica->variaveis[simbolo->id.variavel].first[k], NULL) == -1) {
						plist_append(gramatica->variaveis[producao->origem].first, gramatica->variaveis[simbolo->id.variavel].first[k]);
					}
				}

				// Se este símbolo não gerar vazio, então interrompemos.
				if (!gramatica->variaveis[simbolo->id.variavel].gera_vazio) {
					break;
				}
			}
		}
	}
}

static void calcular_follow(pcc_ll1_t *gramatica) {
	return;
	for (size_t i = 0; i < plist_len(gramatica->producoes); i++) {
		const pcc_producao_t *producao = gramatica->producoes + i;

		for (size_t j = 0; j < plist_len(producao->simbolos); j++) {
			const pcc_simbolo_t *simbolo = producao->simbolos + j;
		}
	}
}
