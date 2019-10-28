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

	/// TODO: talvez dê problema com utf-8
	size_t i, j;
	putchar('\n');
	for (i = 0; linha_src[i] != '\0' && linha_src[i] != '\n'; i++) {
		if (i == coluna) {
			printf("%s", tipo_cor);
		} else if (i == coluna + contexto->lexema_comprimento) {
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
	printf("%s^", tipo_cor);
	for (j = 1; j < contexto->lexema_comprimento && linha_src[j] != '\n'; j++) {
		putchar('~');
	}
	puts(COR(_RESET));
}
