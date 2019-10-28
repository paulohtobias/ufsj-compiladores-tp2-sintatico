/**
 * PCC - Paulo C Compiler
 *
 * log.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <stdarg.h>
#include "log.h"

uint32_t _log_warnings = 0;
uint32_t _log_erros = 0;

void log_mensagem(const char *tipo, const char *tipo_cor, uint32_t *log, const pcc_codigo_fonte_t *fonte, int32_t linha, int32_t coluna, size_t token_comprimento, ...) {
	fprintf(stderr,
		COR(_RESET) COR_NEGRITO(_RESET) "%s:%d:%d: %s%s: " COR(_RESET),
		fonte->caminho, linha, coluna, tipo_cor, tipo
	);

	va_list arg;
	va_start(arg, token_comprimento);
	const char *format = va_arg(arg, const char *);
	vfprintf(stderr, format, arg);
	va_end(arg);

	log_print_linha(pcc_codigo_fonte_get_linha(fonte, linha, 1), token_comprimento, tipo_cor, coluna);

	if (log != NULL) {
		(*log)++;
	}
}

void log_print_linha(const char *linha_src, size_t token_comprimento, const char *cor_tipo, int32_t coluna) {
	coluna--;

	size_t i, j;
	putchar('\n');
	for (i = 0; linha_src[i] != '\0' && linha_src[i] != '\n'; i++) {
		if (i == coluna) {
			printf("%s", cor_tipo);
		} else if (i == coluna + token_comprimento) {
			printf(COR(_RESET));
		}
		putchar(linha_src[i]);
	}

	// Marcador
	putchar('\n');
	for (j = 0; j < coluna; j++) {
		char s = linha_src[j] == '\t' ? '\t' : ' ';
		putchar(s);
	}
	printf("%s^", cor_tipo);
	for (j = 1; j < token_comprimento && linha_src[j] != '\n'; j++) {
		putchar('~');
	}
	puts(COR(_RESET));
}
