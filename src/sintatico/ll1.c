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
#include "pdict.h"
#include "sintatico/ll1.h"

#define COR_TOKEN COR(";40") COR_NEGRITO(_AZUL)
#define COR_VARIAVEL COR(";40") COR_NEGRITO(_VERDE)

int pcc_ll1_g_debug = 0;

static int token_cmp(const void *tk1, const void *tk2);
static void calcular_vazio(pcc_ll1_t *gramatica);
static void calcular_first(pcc_ll1_t *gramatica);
static void calcular_follow(pcc_ll1_t *gramatica);
static void calcular_tabela_M(pcc_ll1_t *gramatica);

void pcc_ll1_criar(pcc_ll1_t *gramatica, char **nome_variaveis) {
	plist_create(gramatica->variaveis, plist_len(nome_variaveis));
	for (uint16_t i = 0; i < plist_len(nome_variaveis); i++) {
		pcc_variavel_t variavel;
		variavel.cod = i;
		variavel.nome = nome_variaveis[i];
		variavel.gera_vazio = false;
		variavel.first = NULL;
		variavel.follow = NULL;
		variavel.M  = NULL;
		plist_put(gramatica->variaveis, variavel, i);
	}

	gramatica->variavel_inicial = -1;

	gramatica->producoes = NULL;
}

void pcc_ll1_liberar(pcc_ll1_t *gramatica) {
	for (size_t i = 0; i < plist_len(gramatica->variaveis); i++) {
		pcc_variavel_t *variavel = gramatica->variaveis + i;

		free(variavel->nome);
		plist_free(variavel->first);
		plist_free(variavel->follow);
		plist_free(variavel->M);
	}
	plist_free(gramatica->variaveis);

	for (size_t i = 0; i < plist_len(gramatica->producoes); i++) {
		plist_free(gramatica->producoes[i].simbolos);
	}
	plist_free(gramatica->producoes);
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
	// Checando se todas as variáveis possuem produção.
	bool *origens = calloc(plist_len(gramatica->variaveis), sizeof *origens);
	for (size_t i = 0; i < plist_len(gramatica->producoes); i++) {
		origens[gramatica->producoes[i].origem] = true;
	}
	for (uint16_t i = 0; i < plist_len(gramatica->variaveis); i++) {
		if (!origens[i]) {
			free(origens);
			LOG_PCC_ERRO(
				1, NULL,
				"Variável " COR_VARIAVEL "%s" COR(_RESET) " não possui produção associada\n",
				gramatica->variaveis[i].nome
			);
		}
	}
	free(origens);

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

	bool alteracao;
	do {
		alteracao = false;
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
						alteracao = true;
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
							alteracao = true;
						}
					}

					// Se este símbolo não gerar vazio, então interrompemos.
					if (!gramatica->variaveis[simbolo->id.variavel].gera_vazio) {
						break;
					}
				}
			}
		}
	} while (alteracao);
}

static void calcular_follow(pcc_ll1_t *gramatica) {
	pcc_simbolo_id_terminal_t $ = {TK_$, TK_$};
	plist_append(gramatica->variaveis[gramatica->variavel_inicial].follow, $);

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
							if (_plist_find(gramatica->variaveis[simbolo_j->id.variavel].follow, &simbolo_k->id.token, token_cmp) == -1) {
								plist_append(gramatica->variaveis[simbolo_j->id.variavel].follow, simbolo_k->id.token);
								alteracao = true;
							}
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
		bool gera_vazio = true;
		for (size_t j = 0; j < plist_len(producao->simbolos); j++) {
			const pcc_simbolo_t *simbolo_j = producao->simbolos + j;

			if (simbolo_j->tipo == SIMBOLO_TERMINAL) {
				struct pcc_ll1_M_t M;
				M.producao_id = producao->id;
				M.token = simbolo_j->id.token;

				int32_t producao_id = _plist_find(gramatica->variaveis[producao->origem].M, &M, token_cmp);
				if (producao_id != -1) {
					if (gramatica->variaveis[producao->origem].M[producao_id].producao_id != producao->id) {
						LOG_PCC_ERRO(0, NULL,
							"gramática não é LL(1) pois o símbolo " COR_TOKEN "%s" COR(_RESET) " pode ser substituído em mais de uma produção:\n",
							token_tipo_subtipo_str(simbolo_j->id.token.tipo, simbolo_j->id.token.subtipo)
						);
						pcc_ll1_producao_print(gramatica, producao);
						pcc_ll1_producao_print(gramatica, &gramatica->producoes[gramatica->variaveis[producao->origem].M[producao_id].producao_id]);
						exit(1);
					}
				} else {
					plist_append(gramatica->variaveis[producao->origem].M, M);
				}
				gera_vazio = false;
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
							LOG_PCC_ERRO(0, NULL,
								"gramática não é LL(1) pois o símbolo " COR_TOKEN "%s" COR(_RESET) " pode ser substituído em mais de uma produção:\n",
								token_tipo_subtipo_str(terminal->tipo, terminal->subtipo)
							);
							pcc_ll1_producao_print(gramatica, producao);
							pcc_ll1_producao_print(gramatica, &gramatica->producoes[gramatica->variaveis[producao->origem].M[producao_id].producao_id]);
							exit(1);
						}
					} else {
						plist_append(gramatica->variaveis[producao->origem].M, M);
					}
				}

				if (!gramatica->variaveis[simbolo_j->id.variavel].gera_vazio) {
					gera_vazio = false;
					break;
				}
			}
		}

		// FOLLOW
		if (gera_vazio) {
			bool follow_$ = false;
			for (size_t j = 0; j < plist_len(gramatica->variaveis[producao->origem].follow); j++) {
				const pcc_simbolo_id_terminal_t *terminal = gramatica->variaveis[producao->origem].follow + j;

				struct pcc_ll1_M_t M;
				M.producao_id = producao->id;
				M.token = *terminal;

				int32_t producao_id = _plist_find(gramatica->variaveis[producao->origem].M, &M, token_cmp);
				if (producao_id != -1) {
					if (gramatica->variaveis[producao->origem].M[producao_id].producao_id != producao->id) {
						LOG_PCC_ERRO(0, NULL,
							"gramática não é LL(1) pois o símbolo " COR_TOKEN "%s" COR(_RESET) " pode ser substituído em mais de uma produção:\n",
							token_tipo_subtipo_str(terminal->tipo, terminal->subtipo)
						);
						pcc_ll1_producao_print(gramatica, producao);
						pcc_ll1_producao_print(gramatica, &gramatica->producoes[gramatica->variaveis[producao->origem].M[producao_id].producao_id]);
						exit(1);
					}
				} else {
					plist_append(gramatica->variaveis[producao->origem].M, M);
				}

				if (terminal->tipo == TK_$ && terminal->subtipo == TK_$) {
					follow_$ = true;
				}
			}

			if (follow_$) {
				struct pcc_ll1_M_t M;
				M.producao_id = producao->id;
				M.token.tipo = TK_$;
				M.token.subtipo = TK_$;

				int32_t producao_id = _plist_find(gramatica->variaveis[producao->origem].M, &M, token_cmp);
				if (producao_id != -1) {
					if (gramatica->variaveis[producao->origem].M[producao_id].producao_id != producao->id) {
						LOG_PCC_ERRO(0, NULL,
							"gramática não é LL(1) pois o símbolo " COR_TOKEN "%s" COR(_RESET) " pode ser substituído em mais de uma produção:\n",
							token_tipo_subtipo_str(M.token.tipo, M.token.subtipo)
						);
						pcc_ll1_producao_print(gramatica, producao);
						pcc_ll1_producao_print(gramatica, &gramatica->producoes[gramatica->variaveis[producao->origem].M[producao_id].producao_id]);
						exit(1);
					}
				} else {
					plist_append(gramatica->variaveis[producao->origem].M, M);
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

void pcc_ll1_de_arquivo(pcc_ll1_t *gramatica, const char *padrao) {
	size_t padrao_comprimento = strlen(padrao);
	char nome_arquivo[padrao_comprimento + 6];
	sprintf(nome_arquivo, "%s.pccg", padrao);
	// Abre o arquivo.
	FILE *in = fopen(nome_arquivo, "r");
	if (in == NULL) {
		LOG_PCC_ERRO(1, NULL, "padrão '%s' não encontrado", padrao);
	}

	uint16_t variaveis_qtd = 0;

	gramatica->producoes = NULL;
	gramatica->variaveis = NULL;
	gramatica->variavel_inicial = 0;

	pdict_t *variaveis = pdict_create();
	char **nome_variaveis = NULL;

	char str[128];

	pcc_producao_t *producao_atual = NULL;
	pcc_producao_t *producoes = NULL;

	const int16_t *indice;

	#define COMENTARIO -1
	#define LINHA_VAZIA -2
	int estado = 0;

	while (fscanf(in, "%s", str) != EOF) {
		// Ignora tudo após um # numa linha.
		if (str[0] == '#') {
			estado = COMENTARIO;
		} else if (str[0] == '\0') {
			estado = LINHA_VAZIA;
		}

		if (estado == 0) {
			indice = pdict_get_value(variaveis, str);
			if (indice == NULL) {
				uint16_t *variavel_indice = malloc(sizeof *variavel_indice);
				*variavel_indice = variaveis_qtd;
				pdict_add_value(variaveis, str, variavel_indice);
				indice = pdict_get_value(variaveis, str);
				plist_append(nome_variaveis, strdup(str));

				variaveis_qtd++;
			}
			plist_append(producoes, (pcc_producao_t) {0});
			producao_atual = producoes + plist_len(producoes) - 1;
			producao_atual->id = plist_len(producoes) - 1;
			producao_atual->origem = *indice;
			producao_atual->simbolos = NULL;
			producao_atual->acao = NULL;

		} else if (estado > 1) {
			pcc_simbolo_t simbolo;
			pcc_simbolo_id_terminal_t token_id;
			if (token_str_tipo_subtipo(str, &token_id.tipo, &token_id.subtipo)) {
				simbolo.tipo = SIMBOLO_TERMINAL;
				simbolo.id.token = token_id;
			} else {
				simbolo.tipo = SIMBOLO_VARIAVEL;
				indice = pdict_get_value(variaveis, str);
				if (indice == NULL) {
					uint16_t *variavel_indice = malloc(sizeof *variavel_indice);
					*variavel_indice = variaveis_qtd;
					pdict_add_value(variaveis, str, variavel_indice);
					indice = pdict_get_value(variaveis, str);
					plist_append(nome_variaveis, strdup(str));

					variaveis_qtd++;
				}
				simbolo.id.variavel = *indice;
			}

			plist_append(producao_atual->simbolos, simbolo);
		}

		estado += estado >= 0;
		if (fgetc(in) == '\n') {
			estado = 0;
		}
	}
	fclose(in);
	#undef COMENTARIO
	#undef LINHA_VAZIA

	pcc_ll1_criar(gramatica, nome_variaveis);

	plist_free(nome_variaveis);
	pdict_destroy(variaveis);

	for (size_t i = 0; i < plist_len(producoes); i++) {
		pcc_ll1_add_producao(gramatica, producoes[i]);
	}
	plist_free(producoes);

	pcc_ll1_calcular(gramatica);
}

static void pilha_inserir(pcc_simbolo_t **pilha, pcc_simbolo_t *simbolos, int32_t simbolos_qtd) {
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

	return (*pilha)[--plist->length];
}

static void pilha_print(const pcc_ll1_t *gramatica, pcc_simbolo_t *pilha) {
	printf("PILHA:");
	if (plist_len(pilha) == 0) {
		return;
	}

	for (ssize_t i = plist_len(pilha) - 1; i >= 0; i--) {
		const pcc_simbolo_t *simbolo = pilha + i;
		if (simbolo->tipo == SIMBOLO_TERMINAL) {
			printf(" " COR_TOKEN "%s", token_tipo_subtipo_str(simbolo->id.token.tipo, simbolo->id.token.subtipo));
		} else {
			printf(" " COR_VARIAVEL "%s", gramatica->variaveis[simbolo->id.variavel].nome);
		}
		printf(COR(_RESET));
	}
}

static void log_erro_esperado(const pcc_ll1_t *gramatica, const pcc_simbolo_t *pilha_topo, const token_t *lista_tokens, ssize_t i) {
	if (pilha_topo->tipo == SIMBOLO_TERMINAL) {
		if (i >= 0) {
			pcc_log_erro(
				&lista_tokens[i].contexto,
				"esperava " COR_TOKEN "%s" COR(_RESET) " antes do token " COR_TOKEN "%s" COR(_RESET),
				token_tipo_subtipo_str(pilha_topo->id.token.tipo, pilha_topo->id.token.subtipo),
				token_tipo_subtipo_str(lista_tokens[i].tipo, lista_tokens[i].subtipo)
			);
		} else {
			i = plist_len(lista_tokens) - 1;
			pcc_log_erro(
				&lista_tokens[i].contexto,
				"esperava " COR_TOKEN "%s" COR(_RESET) " antes do fim inesperado do input",
				token_tipo_subtipo_str(pilha_topo->id.token.tipo, pilha_topo->id.token.subtipo)
			);
		}
	} else {
		const pcc_variavel_t *variavel_topo = &gramatica->variaveis[pilha_topo->id.variavel];

		int err_msg_tamanho = 80 + strlen(token_tipo_subtipo_str(variavel_topo->M[0].token.tipo, variavel_topo->M[0].token.subtipo));
		for (size_t j = 0; j < plist_len(variavel_topo->M); j++) {
			err_msg_tamanho += 40 + strlen(token_tipo_subtipo_str(variavel_topo->M[j].token.tipo, variavel_topo->M[j].token.subtipo));
		}
		char err_msg[err_msg_tamanho];
		int offset = 0;
		sprintf(err_msg, "esperava " COR_TOKEN "%s%n", token_tipo_subtipo_str(variavel_topo->M[0].token.tipo, variavel_topo->M[0].token.subtipo), &err_msg_tamanho);
		offset += err_msg_tamanho;
		if (plist_len(variavel_topo->M) > 1) {
			size_t j;
			for (j = 1; j < plist_len(variavel_topo->M) - 1; j++) {
				sprintf(&err_msg[offset], COR(_RESET) ", " COR_TOKEN "%s%n" COR(_RESET), token_tipo_subtipo_str(variavel_topo->M[j].token.tipo, variavel_topo->M[j].token.subtipo), &err_msg_tamanho);
				offset += err_msg_tamanho;
			}
			sprintf(&err_msg[offset], COR(_RESET) " ou " COR_TOKEN "%s%n" COR(_RESET), token_tipo_subtipo_str(variavel_topo->M[j].token.tipo, variavel_topo->M[j].token.subtipo), &err_msg_tamanho);
			offset += err_msg_tamanho;

		}
		if (i >= 0) {
			sprintf(&err_msg[offset], COR(_RESET) " antes do token " COR_TOKEN "%s" COR(_RESET), token_tipo_subtipo_str(lista_tokens[i].tipo, lista_tokens[i].subtipo));
		} else {
			sprintf(&err_msg[offset], COR(_RESET) " antes do fim inesperado de input");
			i = plist_len(lista_tokens) - 1;
		}

		pcc_log_erro(&lista_tokens[i].contexto, "%s", err_msg);
	}
}

int32_t *pcc_ll1_reconhecer(pcc_ll1_t *gramatica, token_t *lista_tokens) {
	pcc_simbolo_t *pilha = NULL;
	int32_t *acoes = NULL;

	pcc_simbolo_t inicio = {SIMBOLO_VARIAVEL, {.variavel=gramatica->variavel_inicial}};

	pilha_inserir(&pilha, &inicio, 1);

	size_t i = 0, lista_tokens_qtd = plist_len(lista_tokens);
	while (i < lista_tokens_qtd && plist_len(pilha) > 0) {
		if (pcc_ll1_g_debug > 0) {
			printf("ENTRADA:");
			const int entrada_tamanho = 2;
			for (int j = 0; j < entrada_tamanho && i + j < lista_tokens_qtd; j++) {
				printf(" " COR_TOKEN "%s" COR(_RESET), token_tipo_subtipo_str(lista_tokens[i + j].tipo, lista_tokens[i + j].subtipo));
			}
			putchar('\n');
			pilha_print(gramatica, pilha);

			if (pcc_ll1_g_debug > 1) {
				getchar();
			} else {
				putchar('\n');
			}
		}

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
				log_erro_esperado(gramatica, pilha_topo, lista_tokens, i);
				erro = true;
			} else {
				pilha_remover(&pilha);
				plist_append(acoes, -1);
				i++;

				if (pcc_ll1_g_debug > 0) {
					printf("Consumiu terminal\n");
				}
			}
		} else {
			const pcc_variavel_t *variavel_topo = &gramatica->variaveis[pilha_topo->id.variavel];

			int32_t producao_id = _plist_find(variavel_topo->M, &lista_tokens[i], token_cmp);
			if (producao_id == -1) {
				log_erro_esperado(gramatica, pilha_topo, lista_tokens, i);
				erro = true;
			} else {
				const pcc_producao_t *producao = &gramatica->producoes[variavel_topo->M[producao_id].producao_id];

				pilha_remover(&pilha);
				pilha_inserir(&pilha, producao->simbolos, 0);

				plist_append(acoes, producao->id);

				if (pcc_ll1_g_debug > 0) {
					printf("Usada produção ");
					pcc_ll1_producao_print(gramatica, producao);
				}
			}
		}
		if (pcc_ll1_g_debug > 0) {
			if (pcc_ll1_g_debug > 1) {
				getchar();
			} else {
				putchar('\n');
			}
		}

		// Tratamento de erro em modo pânico.
		if (erro) {
			// Se o terminal no topo da pilha não casar com a entrada, ele é desempilhado.
			if (pilha_topo->tipo == SIMBOLO_TERMINAL) {
				pilha_remover(&pilha);
			} else {
				const pcc_variavel_t *variavel_topo = &gramatica->variaveis[pilha_topo->id.variavel];

				/*
				 * Como tokens de sincronização usam-se todos os símbolos em FOLLOW(variavel_topo).
				 * Descartam-se os símbolos de entrada até encontrar-se um elemento de
				 * FOLLOW(variavel_topo), quando também descartamos variavel_topo.
				 * É provável que a análise sintática possa prosseguir.
				 */
				while (i < lista_tokens_qtd) {
					if (_plist_find(variavel_topo->follow, &lista_tokens[i], token_cmp) != -1) {
						pilha_remover(&pilha);
						break;
					}
					i++;
				}
			}
		}
	}

	if (i < lista_tokens_qtd) {
		while (i < lista_tokens_qtd) {
			pcc_log_erro(
				&lista_tokens[i].contexto,
				"token " COR_TOKEN "%s" COR(_RESET) " inválido",
				token_tipo_subtipo_str(lista_tokens[i].tipo, lista_tokens[i].subtipo)
			);
			i++;
		}
	} else {
		while (plist_len(pilha) > 0) {
			// Consome os não terminais que geram $
			const pcc_simbolo_t *pilha_topo = &pilha[plist_len(pilha) - 1];

			if (pilha_topo->tipo == SIMBOLO_TERMINAL) {
				break;
			} else {
				const pcc_variavel_t *variavel_topo = &gramatica->variaveis[pilha_topo->id.variavel];

				pcc_simbolo_id_terminal_t $ = {TK_$, TK_$};

				int32_t producao_id = _plist_find(variavel_topo->M, &$, token_cmp);
				if (producao_id == -1) {
					break;
				}

				pilha_remover(&pilha);
			}
		}
	}

	if (plist_len(pilha) > 0) {
		const pcc_simbolo_t *pilha_topo = &pilha[plist_len(pilha) - 1];

		log_erro_esperado(gramatica, pilha_topo, lista_tokens, -1);
	}

	plist_free(pilha);

	return acoes;
}

void pcc_ll1_simbolo_print(const pcc_ll1_t *gramatica, const pcc_simbolo_t *simbolo) {
	if (simbolo->tipo == SIMBOLO_TERMINAL) {
		printf(" " COR_TOKEN "%s", token_tipo_subtipo_str(simbolo->id.token.tipo, simbolo->id.token.subtipo));
	} else {
		printf(" " COR_VARIAVEL "%s", gramatica->variaveis[simbolo->id.variavel].nome);
	}
	printf(COR(_RESET));
}

void pcc_ll1_producao_print(const pcc_ll1_t *gramatica, const pcc_producao_t *producao) {
	printf("%2d: " COR_VARIAVEL "%s" COR(_RESET) " ->", producao->id, gramatica->variaveis[producao->origem].nome);

	if (plist_len(producao->simbolos) == 0) {
		printf(" " COR_TOKEN "Ɛ" COR(_RESET));
	} else {
		for (size_t j = 0; j < plist_len(producao->simbolos); j++) {
			const pcc_simbolo_t *simbolo = producao->simbolos + j;

			pcc_ll1_simbolo_print(gramatica, simbolo);
		}
	}
	printf(COR(_RESET) "\n");
}

void pcc_ll1_print(const pcc_ll1_t *gramatica) {
	puts("Produções");
	for (size_t i = 0; i < plist_len(gramatica->producoes); i++) {
		const pcc_producao_t *producao = gramatica->producoes + i;

		pcc_ll1_producao_print(gramatica, producao);
	}

	puts("\nVariáveis");
	for (size_t i = 0; i < plist_len(gramatica->variaveis); i++) {
		const pcc_variavel_t *variavel = gramatica->variaveis + i;

		printf("%3zu: " COR_VARIAVEL "%s" COR(_RESET) ":\n", i, variavel->nome);

		printf("\tVazio: %s\n", variavel->gera_vazio ? "sim" : "não");

		printf("\tFIRST:");
		for (size_t j = 0; j < plist_len(variavel->first); j++) {
			const pcc_simbolo_id_terminal_t *terminal = variavel->first + j;

			printf(" " COR_TOKEN "%s" COR(_RESET), token_tipo_subtipo_str(terminal->tipo, terminal->subtipo));
		}

		printf(COR(_RESET) "\n\tFOLLOW:");
		for (size_t j = 0; j < plist_len(variavel->follow); j++) {
			const pcc_simbolo_id_terminal_t *terminal = variavel->follow + j;

			printf(" " COR_TOKEN "%s" COR(_RESET), token_tipo_subtipo_str(terminal->tipo, terminal->subtipo));
		}

		printf(COR(_RESET) "\n\tM:");
		for (size_t j = 0; j < plist_len(variavel->M); j++) {
			const struct pcc_ll1_M_t *M = variavel->M + j;

			printf(COR(_RESET) "\n\t\t%2d: ", M->producao_id);

			printf(" " COR_TOKEN "%s" COR(_RESET), token_tipo_subtipo_str(M->token.tipo, M->token.subtipo));
		}

		puts(COR(_RESET));
	}
	#undef VSTR
}

void pcc_ll1_gramatica_cabecalho() {
	puts(
		"# Paulo C Compiler - Grammar\n"
		"#\n"
		"# Comentários são feitos usando o símbolo '#'. Tudo após ele em uma linha será ignorado.\n"
		"# Linhas vazias também serão ignoradas.\n"
		"#\n"
		"# Uma produção é definida por nao_terminal := [lista_de_simbolos...]\n"
		"# Qualquer símbolo que não consta na lista de tokens (também chamados de terminais)\n"
		"# a seguir será considerado um não-terminal (também chamado de variável).\n"
		"# A lista foi gerada automaticamente e serve apenas como consulta. Qualquer\n"
		"# alteração não afetará o resultado do código.\n"
		"#\n"
		"#\n"
		"# Lista de tokens e seus respectivos valores:"
	);

	for (size_t i = 0; i < TK_COUNT; i++) {
		for (size_t j = 0; j < plist_len(__token_str[i]); j++) {
			const char *str = token_tipo_subtipo_str(i, j);
			printf("# %d, %2d: %s\n", (int) i, (int) j, str);
		}
		puts("#");
	}
}
