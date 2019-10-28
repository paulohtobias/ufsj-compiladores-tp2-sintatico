/**
 * PCC - Paulo C Compiler
 *
 * token/pre-processador.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <ctype.h>
#include <string.h>
#include "lexico/token.h"
#include "lexico/token/pre-processador.h"
#include "utils.h"
#include "plist.h"

/// Tipos de pre-processador em string.
#define TK_PP_SUBTIPO(cod, nome) #nome,
const char __preprocessadores[][32] = {
	TK_PP_SUBTIPOS
};
#undef TK_PP_SUBTIPO
size_t __preprocessadores_quantidade = ARR_TAMANHO(__preprocessadores);


/// Função adicionar
static void preprocessador_adicionar(const void *contexto);

/// Função subtipo_str.
const char *preprocessador_str(uint32_t subtipo);

/// Função to_str.
char *preprocessador_to_str(const void *dados, size_t comprimento);

/// Funções init.
int token_preprocessador_init(afd_t *afd) {
	int res = 0;

	afd_t afd_pp;
	afd_init(&afd_pp, 5);

	// Estado 0
	afd_transicao_t transicoes_0[] = {
		{1, {"#", "#", NULL}},
	};
	afd_pp.estados[0] = afd_criar_estado(transicoes_0, ARR_TAMANHO(transicoes_0), false, NULL);

	// Estado 1
	afd_transicao_t transicoes_1[] = {
		{1, {"[ \t]", "[ \t]", NULL}},
		{2, {"[a-z]", "[a-z]", NULL}},
	};
	plist_append(
		afd_pp.estados,
		afd_criar_estado(transicoes_1, ARR_TAMANHO(transicoes_1), false, NULL)
	);

	// Estado 2
	afd_transicao_t transicoes_2[] = {
		{2, {"[a-z]", "[a-z]", NULL}},
		{5, {"\\n", "\\n", NULL}},
		{3, {"[^a-z]", "[^a-z]", NULL}},
	};
	plist_append(
		afd_pp.estados,
		afd_criar_estado(transicoes_2, ARR_TAMANHO(transicoes_2), false, NULL)
	);

	// Estado 3
	afd_transicao_t transicoes_3[] = {
		{3, {"[^\\\\\\n]", "[^\\\\\\n]", NULL}},
		{4, {"\\\\", "\\\\", NULL}},
		{5, {"\\n", "\\n", NULL}},
	};
	plist_append(
		afd_pp.estados,
		afd_criar_estado(transicoes_3, ARR_TAMANHO(transicoes_3), false, NULL)
	);

	// Estado 4
	afd_transicao_t transicoes_4[] = {
		{3, {"[\\S\\n]", "[\\S\\n]", NULL}},
		{4, {"\\s", "\\s", NULL}},
	};
	plist_append(
		afd_pp.estados,
		afd_criar_estado(transicoes_4, ARR_TAMANHO(transicoes_4), false, NULL)
	);

	// Estado Final
	plist_append(afd_pp.estados, afd_criar_estado(NULL, 0, true, preprocessador_adicionar));

	if ((res = afd_mesclar_automatos(afd, &afd_pp)) != AFD_OK) {
		LOG_PCC_ERRO(0, NULL, "%s: Não foi possível iniciar: %s\n", __FUNCTION__, afd_get_erro(res));
		goto fim;
	}

	afd_liberar(&afd_pp);

	// Inicializando a tabela de símbolos de preprocessador.
	plist_create(tabela_simbolos[TK_PP], __preprocessadores_quantidade);
	memset(tabela_simbolos[TK_PP], 0, __preprocessadores_quantidade * sizeof *tabela_simbolos[TK_PP]);

	// Inicializando a matriz de descrição.
	#define TK_PP_SUBTIPO(cod, nome) \
		plist_append(__token_str[TK_PP], #nome);

	TK_PP_SUBTIPOS
	#undef TK_PP_SUBTIPO
fim:
	return res;
}

static void preprocessador_adicionar(const void *_contexto) {
	const token_contexto_t *contexto = _contexto;
	// Pulando a # e possíveis espaços antes do nome da diretiva.
	int32_t lexema_offset = 1;
	while (isspace((unsigned char) contexto->_lexema[lexema_offset])) {
		lexema_offset++;
	}

	// subtipo é o índice do lexema na tabela.
	int subtipo;
	for (subtipo = 0; subtipo < __preprocessadores_quantidade; subtipo++) {
		size_t i;
		for (i = 0; __preprocessadores[subtipo][i] != '\0' && islower((unsigned char) contexto->_lexema[i + lexema_offset]); i++) {
			if (__preprocessadores[subtipo][i] != contexto->_lexema[i + lexema_offset]) {
				break;
			}
		}

		// Houve casamento.
		if (__preprocessadores[subtipo][i] == '\0' && !islower((unsigned char) contexto->_lexema[i + lexema_offset])) {
			break;
		}
	}

	if (subtipo < __preprocessadores_quantidade) {
		token_t token = token_criar(TK_PP, subtipo, contexto);
		token.subtipo_to_str = preprocessador_str;

		// Copiando a # inicial.
		token.valor.tamanho = 1;
		PMALLOC(token.valor.dados, token.valor.tamanho);
		((char *) token.valor.dados)[0] = '#';

		// Convertendo o lexema para uma única linha.
		int32_t posicao_inicial = lexema_offset;
		int32_t posicao_final = lexema_offset;
		int32_t posicao_barra = -1;

		for (int32_t i = 0; i < token.contexto.lexema_comprimento; i++) {
			if (token.contexto._lexema[i] == '\\') {
				posicao_barra = i;
			} else if (token.contexto._lexema[i] == '\n') {
				if (posicao_barra != -1) {
					if (posicao_barra + 1 < i) {
						pcc_log_warning(&token.contexto, "espaços antes da barra invertida (\\) antes da quebra de linha");
					}

					posicao_final = posicao_barra;
				} else {
					posicao_final = i;
				}

				int32_t offset = token.valor.tamanho;
				token.valor.tamanho += posicao_final - posicao_inicial;
				PREALLOC(token.valor.dados, token.valor.tamanho + 1);
				strncpy(((char *) token.valor.dados) + offset, token.contexto._lexema + posicao_inicial, posicao_final - posicao_inicial);
				posicao_inicial = i + 1;
			} else if (!isspace((unsigned char) token.contexto._lexema[i]) && posicao_barra != -1) {
				posicao_barra = -1;
			}
		}
		((char *) token.valor.dados)[token.valor.tamanho] = '\0';
		token.valor.to_str = preprocessador_to_str;

		token_adicionar(&token);
	} else {
		pcc_log_erro(contexto, "diretiva de pré-processamento inválida");
	}
}

const char *preprocessador_str(uint32_t subtipo) {
	if (subtipo < __preprocessadores_quantidade) {
		return __preprocessadores[subtipo];
	}

	return "Erro: subtipo de preprocessador inválido";
}

char *preprocessador_to_str(const void *dados, size_t comprimento) {
	return strdup(dados);
}
