/**
 * PCC - Paulo C Compiler
 *
 * log.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include "log.h"

uint32_t _log_warnings = 0;
uint32_t _log_erros = 0;

void log_print_linha(const char *linha_src, size_t token_comprimento, const char *cor_tipo, int32_t coluna) {
	coluna--;

	size_t i, j;
	putchar('\n');
	for (i = 0; linha_src[i] != '\0' && linha_src[i] != '\n'; i++) {
		if (i == coluna) {
			printf("%s", cor_tipo);
		}
		putchar(linha_src[i]);
		if (i == coluna + token_comprimento) {
			printf(COR(_RESET));
		}
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
