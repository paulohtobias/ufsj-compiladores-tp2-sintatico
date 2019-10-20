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
#include <ctype.h>
#include "utils.h"
#include "plist.h"
#include "lexico/token.h"
#include "sintatico/ll1.h"

static int token_cmp(const void *tk1, const void *tk2);
static void calcular_vazio(pcc_ll1_t *gramatica);
static void calcular_first(pcc_ll1_t *gramatica);
static void calcular_follow(pcc_ll1_t *gramatica);
static void calcular_tabela_M(pcc_ll1_t *gramatica);

void pcc_ll1_init(pcc_ll1_t *gramatica, uint16_t qtd_variaveis) {
	plist_create(gramatica->variaveis, qtd_variaveis);
	for (uint16_t i = 0; i < plist_cap(gramatica->variaveis); i++) {
		pcc_variavel_t variavel;
		variavel.cod = i;
		variavel.gera_vazio = false;
		variavel.first = NULL;
		variavel.follow = NULL;
		variavel.M  = NULL;
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
	calcular_tabela_M(gramatica);
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

				if (_plist_find(gramatica->variaveis[producao->origem].first, &simbolo->id.token, token_cmp) == -1) {
					plist_append(gramatica->variaveis[producao->origem].first, simbolo->id.token);
				}

				break;
			} else {
				/*
				 * Se for um não-terminal, então adicione FIRST
				 * do símbolo à lista.
				 */

				for (int32_t k = 0; k < plist_len(gramatica->variaveis[simbolo->id.variavel].first); k++) {
					if (_plist_find(gramatica->variaveis[producao->origem].first, &gramatica->variaveis[simbolo->id.variavel].first[k], token_cmp) == -1) {
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
	bool alteracao;
	do {
		alteracao = false;

		for (size_t i = 0; i < plist_len(gramatica->producoes); i++) {
			const pcc_producao_t *producao = gramatica->producoes + i;

			for (size_t j = 0; j < plist_len(producao->simbolos); j++) {
				const pcc_simbolo_t *simbolo_j = producao->simbolos + j;

				if (simbolo_j->tipo == SIMBOLO_VARIAVEL) {
					bool pode_ser_ultimo = true;
					for (size_t k = j + 1; k < plist_len(producao->simbolos); k++) {
						const pcc_simbolo_t *simbolo_k = producao->simbolos + k;

						if (simbolo_k->tipo == SIMBOLO_TERMINAL) {
							plist_append(gramatica->variaveis[simbolo_j->id.variavel].follow, simbolo_k->id.token);
							alteracao = true;
							pode_ser_ultimo = false;
							break;
						} else {
							/*
							 * Se for um não-terminal, então adicione FIRST
							 * do símbolo à lista.
							 */

							for (int32_t l = 0; l < plist_len(gramatica->variaveis[simbolo_k->id.variavel].first); l++) {
								if (_plist_find(gramatica->variaveis[simbolo_j->id.variavel].follow, &gramatica->variaveis[simbolo_k->id.variavel].first[l], token_cmp) == -1) {
									plist_append(gramatica->variaveis[simbolo_j->id.variavel].follow, gramatica->variaveis[simbolo_k->id.variavel].first[l]);
									alteracao = true;
								}
							}

							// Se este símbolo não gerar vazio, então interrompemos.
							if (!gramatica->variaveis[simbolo_k->id.variavel].gera_vazio) {
								pode_ser_ultimo = false;
								break;
							}
						}
					}

					if (pode_ser_ultimo) {
						// Adiciona o FOLLOW de producao->origem em simbolo_j.
						for (int32_t l = 0; l < plist_len(gramatica->variaveis[producao->origem].follow); l++) {
							if (_plist_find(gramatica->variaveis[simbolo_j->id.variavel].follow, &gramatica->variaveis[producao->origem].follow[l], token_cmp) == -1) {
								plist_append(gramatica->variaveis[simbolo_j->id.variavel].follow, gramatica->variaveis[producao->origem].follow[l]);
								alteracao = true;
							}
						}
					}
				}
			}
		}
	} while (alteracao);
}

static void calcular_tabela_M(pcc_ll1_t *gramatica) {
	for (size_t i = 0; i < plist_len(gramatica->producoes); i++) {
		const pcc_producao_t *producao = gramatica->producoes + i;

		for (size_t j = 0; j < plist_len(producao->simbolos); j++) {
			const pcc_simbolo_t *simbolo_j = producao->simbolos + j;

			if (simbolo_j->tipo == SIMBOLO_TERMINAL) {
				struct pcc_ll1_M_t M;
				M.producao_id = producao->origem;
				M.token = simbolo_j->id.token;

				plist_append(gramatica->variaveis[producao->origem].M, M);
			} else {
				for (size_t k = 0; k < plist_len(gramatica->variaveis[simbolo_j->id.variavel].first); k++) {
					const pcc_simbolo_id_terminal_t *terminal = gramatica->variaveis[simbolo_j->id.variavel].first + k;

					struct pcc_ll1_M_t M;
					M.producao_id = producao->origem;
					M.token = *terminal;

					plist_append(gramatica->variaveis[producao->origem].M, M);
				}

				if (!gramatica->variaveis[simbolo_j->id.variavel].gera_vazio) {
					break;
				}
			}
		}
	}
}

static int token_cmp(const void *tk1, const void *tk2) {
	const pcc_simbolo_id_terminal_t *token1 = tk1;
	const pcc_simbolo_id_terminal_t *token2 = tk2;

	int td = token1->tipo - token2->tipo;
	if (td != 0) {
		return td;
	}

	int std = token1->subtipo - token2->subtipo;
	if (std != 0) {
		return std;
	}

	return 0;
}

void pcc_ll1_print(const pcc_ll1_t *gramatica, const char *_variaveis_str, size_t variavel_str_tam) {
	#define VSTR(i) (_variaveis_str + (i) * variavel_str_tam)
	for (size_t i = 0; i < plist_len(gramatica->producoes); i++) {
		const pcc_producao_t *producao = gramatica->producoes + i;

		printf("%2d:" COR_NEGRITO(_VERDE) " %-2s " COR(_RESET) "->", producao->id, VSTR(producao->origem));

		if (plist_len(producao->simbolos) == 0) {
			printf(COR_NEGRITO(_AZUL) " Ɛ" COR(_RESET));
		} else {
			for (size_t j = 0; j < plist_len(producao->simbolos); j++) {
				const pcc_simbolo_t *simbolo = producao->simbolos + j;

				if (simbolo->tipo == SIMBOLO_TERMINAL) {
					printf(COR_NEGRITO(_AZUL));
					token_t token;
					token.tipo = simbolo->id.token.tipo;
					token.subtipo = simbolo->id.token.subtipo;

					putchar(' ');
					for (const char *str = token_tipo_str(&token); *str != '\0'; str++) {
						putchar(tolower(*str));
					}
				} else {
					printf(COR_NEGRITO(_VERDE));
					printf(" %s", VSTR(simbolo->id.variavel));
				}
			}
		}

		printf(COR(_RESET) "\n");
	}

	for (size_t i = 0; i < plist_len(gramatica->variaveis); i++) {
		const pcc_variavel_t *variavel = gramatica->variaveis + i;

		printf(COR_NEGRITO(_VERDE) "%-2s" COR(_RESET) ":\n", VSTR(variavel->cod));

		printf("\tVazio: %s\n", variavel->gera_vazio ? "sim" : "não");

		printf("\tFIRST: ");
		for (size_t j = 0; j < plist_len(variavel->first); j++) {
			const pcc_simbolo_id_terminal_t *terminal = variavel->first + j;

			token_t token;
			token.tipo = terminal->tipo;
			token.subtipo = terminal->subtipo;

			printf(COR_NEGRITO(_AZUL) " ");
			for (const char *str = token_tipo_str(&token); *str != '\0'; str++) {
				putchar(tolower(*str));
			}
		}

		printf(COR(_RESET) "\n\tFOLLOW: ");
		for (size_t j = 0; j < plist_len(variavel->follow); j++) {
			const pcc_simbolo_id_terminal_t *terminal = variavel->follow + j;

			token_t token;
			token.tipo = terminal->tipo;
			token.subtipo = terminal->subtipo;

			printf(COR_NEGRITO(_AZUL) " ");
			for (const char *str = token_tipo_str(&token); *str != '\0'; str++) {
				putchar(tolower(*str));
			}
		}

		printf(COR(_RESET) "\n\tM:");
		for (size_t j = 0; j < plist_len(variavel->M); j++) {
			const struct pcc_ll1_M_t *M = variavel->M + j;

			printf(COR(_RESET) "\n\t\t%2d: ", M->producao_id);

			token_t token;
			token.tipo = M->token.tipo;
			token.subtipo = M->token.subtipo;

			printf(COR_NEGRITO(_AZUL) " ");
			for (const char *str = token_tipo_str(&token); *str != '\0'; str++) {
				putchar(tolower(*str));
			}
		}

		puts(COR(_RESET));
	}
	#undef VSTR
}
