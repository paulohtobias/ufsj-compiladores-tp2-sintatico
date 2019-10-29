/**
 * PCC - Paulo C Compiler
 *
 * log.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include "lexico/token.h"
#include "log.h"

/// Contador de warnings.
uint32_t _log_warnings = 0;

/// Contador de erros.
uint32_t _log_erros = 0;

void pcc_log_warning(const void *contexto, const char *fmt, ...) {
	_log_warnings++;

	va_list arg;
	va_start(arg, fmt);
	pcc_log_mensagem("warning", COR_NEGRITO(_AMARELO), contexto, fmt, arg);
	va_end(arg);
}

void pcc_log_erro(const void *contexto, const char *fmt, ...) {
	_log_erros++;

	va_list arg;
	va_start(arg, fmt);
	pcc_log_mensagem("erro", COR_NEGRITO(_VERMELHO), contexto, fmt, arg);
	va_end(arg);
}

void pcc_log_mensagem(const char *tipo, const char *tipo_cor, const void *_contexto, const char *fmt, va_list arg) {
	const token_contexto_t *contexto = _contexto;
	int32_t linha = contexto->posicao.linha;
	int32_t coluna = contexto->posicao.coluna;

	// Mensagem de erro.
	fprintf(stderr,
		COR(_RESET) COR_NEGRITO(_RESET) "%s:%d:%d: %s%s: " COR(_RESET),
		contexto->fonte->caminho, linha, coluna, tipo_cor, tipo
	);

	vfprintf(stderr, fmt, arg);

	// Printando a linha com marcador de erro.
	const char *linha_src = pcc_codigo_fonte_get_linha(contexto->fonte, linha, 1);
	coluna--;

	size_t i, j;
	int c = contexto->lexema_comprimento + 1;
	putchar('\n');
	for (i = 0, j = 0; linha_src[i] != '\0' && linha_src[i] != '\n';) {
		int8_t comprimento = utf8_simbolo_comprimento(linha_src + i);

		if (j == coluna) {
			printf("%s", tipo_cor);
			c = 0;
		} else if (c == contexto->lexema_comprimento) {
			printf(COR(_RESET));
		}
		printf("%.*s", comprimento, linha_src + i);

		j++;
		i += comprimento;

		c += (c < contexto->lexema_comprimento) * comprimento;
	}

	// Marcador
	putchar('\n');
	j = 0;
	for (i = 0; j < coluna; j++) {
		int8_t comprimento = utf8_simbolo_comprimento(linha_src + i);

		putchar(linha_src[i] == '\t' ? '\t' : ' ');
		if (comprimento > 2) {
			putchar(' ');
		}

		i += comprimento;
	}
	printf("%s^", tipo_cor);
	i = utf8_simbolo_comprimento(contexto->_lexema);
	while (i < contexto->lexema_comprimento && linha_src[i] != '\n') {
		putchar('~');

		i += utf8_simbolo_comprimento(contexto->_lexema + i);
	}
	puts(COR(_RESET));
}
