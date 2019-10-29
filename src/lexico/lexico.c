/**
 * PCC - Paulo C Compiler
 *
 * lexico.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "lexico/codigo_fonte.h"
#include "lexico/lexico.h"

afd_t *afd_lexico = NULL;
int lexico_init() {
	int res = 0;

	pcc_codigo_fonte_init();

	PMALLOC(afd_lexico, 1);

	if ((res = afd_init(afd_lexico, 1)) != AFD_OK) {
		LOG_PCC_ERRO(res, NULL, "Erro ao inicializar o AFD: %s\n", afd_get_erro(res));
		return res;
	}

	// Inicializando os módulos de token.
	if ((res = token_init(afd_lexico)) != 0) {
		return 1;
	}

	return 0;
}

void lexico_finalizar() {
	token_finalizar();

	afd_liberar(afd_lexico);
	free(afd_lexico);
}

/// Avança no código fonte. Retorna true se houve um '\n'
static bool avancar_cursor(char **src, int8_t comprimento, int32_t *linha, int32_t *coluna, token_contexto_t *contexto) {
	bool nl = false;

	if (comprimento <= 0) {
		comprimento = utf8_simbolo_comprimento(*src);
	}

	if (**src == '\n') {
		nl = true;
		(*linha)++;
		(*coluna) = 1;
	} else {
		(*coluna)++;
	}

	(*src) += comprimento;

	if (contexto != NULL) {
		contexto->lexema_comprimento += comprimento;
	}

	return nl;
}

static void ignorar_espacos(char **src, int32_t *linha, int32_t *coluna, token_contexto_t *contexto) {
	while (isspace((unsigned int) **src)) {
		avancar_cursor(src, -1, linha, coluna, NULL);
	}
}

int lexico_parse(const char *nome_arquivo) {
	// Carregando o arquivo para a memória. TODO: usar mmap
	pcc_codigo_fonte_t *codigo_fonte = pcc_codigo_fonte_abir(nome_arquivo);

	char *src = codigo_fonte->src;

	// Variáveis de contexto.
	int32_t linha = 1;
	int32_t coluna = 1;
	token_contexto_t contexto;

	// Ignorando espaços iniciais.
	ignorar_espacos(&src, &linha, &coluna, &contexto);

	// Inicializando o contexto.
	contexto.fonte = codigo_fonte;
	contexto.posicao.linha = linha;
	contexto.posicao.coluna = coluna;
	contexto._lexema = pcc_codigo_fonte_get_linha(contexto.fonte, contexto.posicao.linha, contexto.posicao.coluna);
	contexto.lexema_comprimento = 0;

	// Estado inicial.
	afd_estado_t *estado_atual = afd_lexico->estados;

	int8_t simbolo_comprimento = 1;
	bool moveu = false;
	while (*src != '\0') {
		simbolo_comprimento = utf8_simbolo_comprimento(src);
		int32_t estado_indice = afd_estado_get_proximo_estado(estado_atual, src, simbolo_comprimento);

		if (estado_indice >= 0) {
			estado_atual = &afd_lexico->estados[estado_indice];
			avancar_cursor(&src, simbolo_comprimento, &linha, &coluna, &contexto);
			moveu = true;
		} else {
			/*
			 * estado_atual->acao != NULL pode significar duas coisas:
			 *   1: estado é final e um novo token será criado.
			 *   2: estado não é final mas possui mensagem de erro contextualizada.
			 *
			 * Ambas funções (criar novo estado e logar erro) possuem mesma assinatura.
			 * Portanto, não é necessário verificar se o estado é final.
			 */
			if (estado_atual->acao != NULL) {
				estado_atual->acao(&contexto);

				/*
				 * Se houve uma transição antes do erro, então o símbolo é
				 * considerado como um separador e não será consumido.
				 */
				if (!moveu) {
					avancar_cursor(&src, simbolo_comprimento, &linha, &coluna, &contexto);
				}
			} else {
				if (!moveu) {
					avancar_cursor(&src, simbolo_comprimento, &linha, &coluna, &contexto);
					pcc_log_erro(
						&contexto,
						"símbolo '%.*s' inválido", simbolo_comprimento, contexto._lexema
					);
				}
			}

			// Estado atual volta a ser o inicial.
			estado_atual = afd_lexico->estados;

			// Resetando o contexto.
			ignorar_espacos(&src, &linha, &coluna, &contexto);
			if (*src == '\0') {
				break;
			}

			contexto.posicao.linha = linha;
			contexto.posicao.coluna = coluna;
			contexto._lexema = pcc_codigo_fonte_get_linha(contexto.fonte, contexto.posicao.linha, contexto.posicao.coluna);
			contexto.lexema_comprimento = 0;

			moveu = false;
		}
	}

	// Se existia alguma ação a ser executada.
	if (estado_atual->acao != NULL) {
		estado_atual->acao(&contexto);
	}

	return 0;
}
