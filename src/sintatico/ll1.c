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
#include "log.h"
#include "plist.h"
#include "lexico/lexico.h"
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
			if (_plist_find(gramatica->variaveis[producao->origem].first, &producao->simbolos[0].id.token, token_cmp) == -1) {
				plist_append(gramatica->variaveis[producao->origem].first, producao->simbolos[0].id.token);
			}
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

		// FIRST
		for (size_t j = 0; j < plist_len(producao->simbolos); j++) {
			const pcc_simbolo_t *simbolo_j = producao->simbolos + j;

			if (simbolo_j->tipo == SIMBOLO_TERMINAL) {
				struct pcc_ll1_M_t M;
				M.producao_id = producao->id;
				M.token = simbolo_j->id.token;

				/// TODO: debugar essas verificações (linha 232 tbm)
				int32_t producao_id = _plist_find(gramatica->variaveis[producao->origem].M, &M, token_cmp);
				if (producao_id != -1) {
					if (gramatica->variaveis[producao->origem].M[producao_id].producao_id != producao->id) {
						LOG_PCC_ERRO(1, NULL,
							"gramática não é LL(1) pois o símbolo %s pode ser substituído em mais de uma produção (%d) e (%d)",
							simbolo_j->id.token.str, gramatica->variaveis[producao->origem].M[producao_id].producao_id, producao->id
						);
					}
				} else {
					plist_append(gramatica->variaveis[producao->origem].M, M);
				}
				break;
			} else {
				for (size_t k = 0; k < plist_len(gramatica->variaveis[simbolo_j->id.variavel].first); k++) {
					const pcc_simbolo_id_terminal_t *terminal = gramatica->variaveis[simbolo_j->id.variavel].first + k;

					struct pcc_ll1_M_t M;
					M.producao_id = producao->id;
					M.token = *terminal;

					int32_t producao_id = _plist_find(gramatica->variaveis[producao->origem].M, &M, token_cmp);
					if (producao_id != -1) {
						if (gramatica->variaveis[producao->origem].M[producao_id].producao_id != producao->id) {
							LOG_PCC_ERRO(1, NULL,
								"gramática não é LL(1) pois o símbolo %s pode ser substituído em mais de uma produção (%d) e (%d)",
								terminal->str, gramatica->variaveis[producao->origem].M[producao_id].producao_id, producao->id
							);
						}
					} else {
						plist_append(gramatica->variaveis[producao->origem].M, M);
					}
				}

				if (!gramatica->variaveis[simbolo_j->id.variavel].gera_vazio) {
					break;
				}
			}
		}

		// FOLLOW
		for (size_t j = 0; j < plist_len(producao->simbolos); j++) {
			const pcc_simbolo_t *simbolo_j = producao->simbolos + j;
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

static void pilha_inserir(pcc_simbolo_t **pilha, pcc_simbolo_t *simbolos, size_t simbolos_qtd) {
	if (simbolos_qtd == 0) {
		simbolos_qtd = plist_len(simbolos);
	}

	for (int32_t i = simbolos_qtd - 1; i >= 0; i--) {
		plist_append(*pilha, simbolos[i]);
	}
}

static pcc_simbolo_t pilha_remover(pcc_simbolo_t **pilha) {
	plist_t *plist = __PLIST_L2P(*pilha);

	// Não deve acontecer nunca.
	if (plist->length == 0) {
		return (pcc_simbolo_t) {0};
	}

	return (*pilha)[plist->length--];
}

void pcc_ll1_reconhecer(pcc_ll1_t *gramatica, token_t *lista_tokens) {
	pcc_simbolo_t *pilha = NULL;

	pcc_simbolo_t inicio = {SIMBOLO_VARIAVEL, {.variavel=gramatica->variavel_inicial}};

	pilha_inserir(&pilha, &inicio, 1);

	size_t i = 0, lista_tokens_qtd = plist_len(lista_tokens);
	while (i < lista_tokens_qtd) {
		bool erro = false;

		// Se o topo da pilha for um terminal
		const pcc_simbolo_t *pilha_topo = &pilha[plist_len(pilha) - 1];
		if (pilha_topo->tipo == SIMBOLO_TERMINAL) {

			/*
			 * Se os símbolos forem diferentes, isto é um erro.
			 * Note que, apesar de pilha_topo->id.token e lista_tokens[i]
			 * serem de tipos diferentes, a comparação é possível.
			 */
			if (token_cmp(&pilha_topo->id.token, &lista_tokens[i])) {
				// TODO: melhorar mensagem de erro.
				LOG_ERRO(
					lista_tokens[i].contexto.arquivo,
					lista_tokens[i].contexto.posicao.linha, lista_tokens[i].contexto.posicao.coluna,
					lista_tokens[i].contexto.lexema, lista_tokens[i].contexto.comprimento,
					"símbolo inesperado");
				erro = true;
			} else {
				pilha_remover(&pilha);
				i++;
			}
		} else {
			/// TODO: tratar substituição de variável.

			const pcc_variavel_t *variavel_topo = &gramatica->variaveis[pilha_topo->id.variavel];

			int32_t producao_id = _plist_find(variavel_topo->M, &lista_tokens[i], token_cmp);
			if (producao_id == -1) {
				// TODO: melhorar mensagem de erro.
				LOG_ERRO(
					lista_tokens[i].contexto.arquivo,
					lista_tokens[i].contexto.posicao.linha, lista_tokens[i].contexto.posicao.coluna,
					lista_tokens[i].contexto.lexema, lista_tokens[i].contexto.comprimento,
					"não existe uma produção para este símbolo");

				erro = true;
			} else {
				const pcc_producao_t *producao = &gramatica->producoes[variavel_topo->M[producao_id].producao_id];

				pilha_remover(&pilha);
				pilha_inserir(&pilha, producao->simbolos, 0);
			}
		}

		// Se houve um erro, avança até um ; e recomeça a análise a partir
		// do próximo token.
		if (erro) {
			while (i < lista_tokens_qtd && !(lista_tokens[i].tipo == TK_EXT && lista_tokens[i].subtipo == TK_EXT_PT_VIRGULA)) {
				i++;
			}
			i++;

			/// TODO: precisa mexer na pilha também? SIM
			while (plist_len(pilha) > 0) {
				pilha_remover(&pilha);
			}
			pilha_inserir(&pilha, &inicio, 1);
		}
	}

	/// TODO: checar se a pilha ficou vazia.

	printf("Reconhecido\n");
}

void pcc_ll1_print(const pcc_ll1_t *gramatica, const char *_variaveis_str, size_t variavel_str_tam) {
	#define VSTR(i) (_variaveis_str + (i) * variavel_str_tam)
	for (size_t i = 0; i < plist_len(gramatica->producoes); i++) {
		const pcc_producao_t *producao = gramatica->producoes + i;

		printf("%2d: " COR(";40") COR_NEGRITO(_VERDE) "%s" COR(_RESET) " ->", producao->id, VSTR(producao->origem));

		if (plist_len(producao->simbolos) == 0) {
			printf(" " COR(";40") COR_NEGRITO(_AZUL) "Ɛ" COR(_RESET));
		} else {
			for (size_t j = 0; j < plist_len(producao->simbolos); j++) {
				const pcc_simbolo_t *simbolo = producao->simbolos + j;

				if (simbolo->tipo == SIMBOLO_TERMINAL) {
					printf(" " COR(";40") COR_NEGRITO(_AZUL) "%s", simbolo->id.token.str);
				} else {
					printf(" " COR(";40") COR_NEGRITO(_VERDE) "%s", VSTR(simbolo->id.variavel));
				}
				printf(COR(_RESET));
			}
		}

		printf(COR(_RESET) "\n");
	}

	for (size_t i = 0; i < plist_len(gramatica->variaveis); i++) {
		const pcc_variavel_t *variavel = gramatica->variaveis + i;

		printf(COR(";40") COR_NEGRITO(_VERDE) "%s" COR(_RESET) ":\n", VSTR(variavel->cod));

		printf("\tVazio: %s\n", variavel->gera_vazio ? "sim" : "não");

		printf("\tFIRST:");
		for (size_t j = 0; j < plist_len(variavel->first); j++) {
			const pcc_simbolo_id_terminal_t *terminal = variavel->first + j;

			printf(" " COR(";40") COR_NEGRITO(_AZUL) "%s" COR(_RESET), terminal->str);
		}

		printf(COR(_RESET) "\n\tFOLLOW:");
		for (size_t j = 0; j < plist_len(variavel->follow); j++) {
			const pcc_simbolo_id_terminal_t *terminal = variavel->follow + j;

			printf(" " COR(";40") COR_NEGRITO(_AZUL) "%s" COR(_RESET), terminal->str);
		}

		printf(COR(_RESET) "\n\tM:");
		for (size_t j = 0; j < plist_len(variavel->M); j++) {
			const struct pcc_ll1_M_t *M = variavel->M + j;

			printf(COR(_RESET) "\n\t\t%2d: ", M->producao_id);

			printf(" " COR(";40") COR_NEGRITO(_AZUL) "%s" COR(_RESET), M->token.str);
		}

		puts(COR(_RESET));
	}
	#undef VSTR
}
