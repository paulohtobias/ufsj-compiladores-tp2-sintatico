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

void log_print_linha(const char *src, size_t comprimento, const char *cor_tipo, int32_t coluna) {
	// TODO
	putchar('\n');

	size_t i = 0;
	while (src[i] != '\0' && src[i] != '\n') {
		if (i == coluna - 1) {
			printf("%s", cor_tipo);
		}
		putchar(src[i]);
		if (i == coluna - 1) {
			printf(COR(_RESET));
		}
		i++;
	}
	while (i < comprimento) {
		putchar(src[i]);
		i++;
	}

	puts("\n");
}
