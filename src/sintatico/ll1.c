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
#include "lexico/lexico.h"
#include "sintatico/ll1.h"

#define COR_TOKEN COR(";40") COR_NEGRITO(_AZUL)
#define COR_VARIAVEL COR(";40") COR_NEGRITO(_VERDE)

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
	// Checando se todas as variáveis possuem produção.
	bool *origens = calloc(plist_len(gramatica->variaveis), sizeof *origens);
	for (size_t i = 0; i < plist_len(gramatica->producoes); i++) {
		origens[gramatica->producoes[i].origem] = true;
	}
	for (uint16_t i = 0; i < plist_len(gramatica->variaveis); i++) {
		if (!origens[i]) {
			LOG_PCC_ERRO(1, NULL, "Variável %d não possui produção associada\n", i)
		}
	}

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
						LOG_PCC_ERRO(1, NULL,
							"gramática não é LL(1) pois o símbolo %s pode ser substituído em mais de uma produção (%d) e (%d)",
							token_tipo_subtipo_str(simbolo_j->id.token.tipo, simbolo_j->id.token.subtipo), gramatica->variaveis[producao->origem].M[producao_id].producao_id, producao->id
						);
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
							LOG_PCC_ERRO(1, NULL,
								"gramática não é LL(1) pois o símbolo %s pode ser substituído em mais de uma produção (%d) e (%d)",
								token_tipo_subtipo_str(terminal->tipo, terminal->subtipo), gramatica->variaveis[producao->origem].M[producao_id].producao_id, producao->id
							);
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
			for (size_t j = 0; j < plist_len(gramatica->variaveis[producao->origem].follow); j++) {
				const pcc_simbolo_id_terminal_t *terminal = gramatica->variaveis[producao->origem].follow + j;

				struct pcc_ll1_M_t M;
				M.producao_id = producao->id;
				M.token = *terminal;

				int32_t producao_id = _plist_find(gramatica->variaveis[producao->origem].M, &M, token_cmp);
				if (producao_id != -1) {
					if (gramatica->variaveis[producao->origem].M[producao_id].producao_id != producao->id) {
						LOG_PCC_ERRO(1, NULL,
							"gramática não é LL(1) pois o símbolo %s pode ser substituído em mais de uma produção (%d) e (%d)",
							token_tipo_subtipo_str(terminal->tipo, terminal->subtipo), gramatica->variaveis[producao->origem].M[producao_id].producao_id, producao->id
						);
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

void pcc_ll1_de_arquivo(pcc_ll1_t *gramatica, const char *nome_arquivo) {
	// Abre o arquivo.
	FILE *in = fopen(nome_arquivo, "r");
	if (in == NULL) {
		LOG_PCC_ERRO(1, "não foi possível abrir o arquivo", " ");
	}

	uint16_t variaveis_qtd = 0;

	gramatica->producoes = NULL;
	gramatica->variaveis = NULL;
	gramatica->variavel_inicial = 0;

	/// TODO: usar a pdict_create_all com um tamanho máximo grande o suficiente pra caber todas as variáveis
	/// pra evitar recopiar o vetor.
	pdict_t *variaveis = pdict_create();

	char str[128];

	pcc_producao_t producao_atual = (pcc_producao_t) {0};
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

				variaveis_qtd++;
			}
			producao_atual.origem = *indice;
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


					variaveis_qtd++;
				}
				simbolo.id.variavel = *indice;
			}

			plist_append(producao_atual.simbolos, simbolo);
		}

		estado += estado >= 0;
		if (fgetc(in) == '\n') {
			if (estado >= 0) {
				plist_append(producoes, producao_atual);
			}

			estado = 0;
			producao_atual.simbolos = NULL;
		}
	}
	fclose(in);
	#undef COMENTARIO
	#undef LINHA_VAZIA

	int32_t keys_len;
	char **variaveis_str = pdict_get_keys(variaveis, &keys_len, true);

	pcc_ll1_init(gramatica, variaveis_qtd);

	for (size_t i = 0; i < plist_len(producoes); i++) {
		pcc_ll1_add_producao(gramatica, producoes[i]);
	}

	pcc_ll1_calcular(gramatica);

	pcc_ll1_print(gramatica, (const char **) variaveis_str);
}

void pcc_ll1_reconhecer(pcc_ll1_t *gramatica, token_t *lista_tokens) {
	pcc_simbolo_t *pilha = NULL;
	int32_t *acoes = NULL;

	pcc_simbolo_t inicio = {SIMBOLO_VARIAVEL, {.variavel=gramatica->variavel_inicial}};

	pilha_inserir(&pilha, &inicio, 1);

	size_t i = 0, lista_tokens_qtd = plist_len(lista_tokens);
	while (i < lista_tokens_qtd && plist_len(pilha) > 0) {
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
					"esperava " COR_TOKEN "%s" COR(_RESET) " antes do token " COR_TOKEN "%s" COR(_RESET),
					token_tipo_subtipo_str(pilha_topo->id.token.tipo, pilha_topo->id.token.subtipo),
					token_tipo_subtipo_str(lista_tokens[i].tipo, lista_tokens[i].subtipo)
				);
				erro = true;
			} else {
				pilha_remover(&pilha);
				plist_append(acoes, -1);
				i++;
			}
		} else {
			const pcc_variavel_t *variavel_topo = &gramatica->variaveis[pilha_topo->id.variavel];

			int32_t producao_id = _plist_find(variavel_topo->M, &lista_tokens[i], token_cmp);
			if (producao_id == -1) {
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
				sprintf(&err_msg[offset], COR(_RESET) " antes do token " COR_TOKEN "%s" COR(_RESET), token_tipo_subtipo_str(lista_tokens[i].tipo, lista_tokens[i].subtipo));

				// TODO: melhorar mensagem de erro.
				LOG_ERRO(
					lista_tokens[i].contexto.arquivo,
					lista_tokens[i].contexto.posicao.linha, lista_tokens[i].contexto.posicao.coluna,
					lista_tokens[i].contexto.lexema, lista_tokens[i].contexto.comprimento,
					"%s", err_msg
				);
				erro = true;
			} else {
				const pcc_producao_t *producao = &gramatica->producoes[variavel_topo->M[producao_id].producao_id];

				pilha_remover(&pilha);
				pilha_inserir(&pilha, producao->simbolos, 0);

				plist_append(acoes, producao->id);
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

	/// TODO: remover todo mundo da pilha que gera vazio.
	if (i < lista_tokens_qtd) {
		LOG_PCC_ERRO(1, NULL, "Há tokens %zu não reconhecidos.\n", lista_tokens_qtd - i);
	}

	/// TODO: checar se a pilha ficou vazia ou ainda sobraram tokenss.
	while (plist_len(pilha) > 0) {
		const pcc_simbolo_t *pilha_topo = &pilha[plist_len(pilha) - 1];

		if (pilha_topo->tipo == SIMBOLO_TERMINAL) {
			LOG_PCC_ERRO(1, NULL, "Há terminais na pilha.\n");
		}

		if (gramatica->variaveis[pilha_topo->id.variavel].gera_vazio) {
			pilha_remover(&pilha);
		} else {
			LOG_PCC_ERRO(0, NULL, "Sobraram %zu elementos na pilha.\n", plist_len(pilha));
			for (int32_t i = plist_len(pilha) - 1; i >= 0; i--) {
				if (pilha[i].tipo == SIMBOLO_TERMINAL) {
					printf(" " COR_TOKEN "%s", token_tipo_subtipo_str(pilha[i].id.token.tipo, pilha[i].id.token.subtipo));
				} else {
					printf(" " COR_VARIAVEL "%d", pilha[i].id.variavel);
				}
				printf(COR(_RESET));
			}
			printf(COR(_RESET) "\n");
			exit(1);
		}
	}

	printf("Reconhecido\n");
}

void pcc_ll1_print(const pcc_ll1_t *gramatica, const char **variaveis_str) {
	puts("Produções");
	for (size_t i = 0; i < plist_len(gramatica->producoes); i++) {
		const pcc_producao_t *producao = gramatica->producoes + i;

		printf("%2d: " COR_VARIAVEL "%s" COR(_RESET) " ->", producao->id, variaveis_str[producao->origem]);

		if (plist_len(producao->simbolos) == 0) {
			printf(" " COR_TOKEN "Ɛ" COR(_RESET));
		} else {
			for (size_t j = 0; j < plist_len(producao->simbolos); j++) {
				const pcc_simbolo_t *simbolo = producao->simbolos + j;

				if (simbolo->tipo == SIMBOLO_TERMINAL) {
					printf(" " COR_TOKEN "%s", token_tipo_subtipo_str(simbolo->id.token.tipo, simbolo->id.token.subtipo));
				} else {
					printf(" " COR_VARIAVEL "%s", variaveis_str[simbolo->id.variavel]);
				}
				printf(COR(_RESET));
			}
		}

		printf(COR(_RESET) "\n");
	}

	puts("\nVariáveis");
	for (size_t i = 0; i < plist_len(gramatica->variaveis); i++) {
		const pcc_variavel_t *variavel = gramatica->variaveis + i;

		printf("%3zu: " COR_VARIAVEL "%s" COR(_RESET) ":\n", i, variaveis_str[variavel->cod]);

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
