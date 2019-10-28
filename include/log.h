/**
 * PCC - Paulo C Compiler
 *
 * log.h
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#include "utils.h"

// Erros do código.
void pcc_log_mensagem(const char *tipo, const char *tipo_cor, const void *contexto, const char *fmt, va_list arg);

void pcc_log_warning(const void *contexto, const char *fmt, ...);

void pcc_log_erro(const void *contexto, const char *fmt, ...);


// Erros do compilador.
#define LOG_PCC(tipo, COR_TIPO, encerrar, perror_mensagem, ...) \
	do { \
		fprintf(stderr, \
			COR(_RESET) COR_NEGRITO(_RESET) "pcc: " COR(_RESET) COR_NEGRITO(COR_TIPO) "%s: " COR(_RESET), \
			tipo \
		); \
		fprintf(stderr, __VA_ARGS__); \
		if (perror_mensagem != NULL) { \
			perror(perror_mensagem); \
		} else { \
			fprintf(stderr, "\n"); \
		} \
		if (encerrar) { \
			exit(encerrar); \
		} \
	} while(0);

#define LOG_PCC_ERRO(encerrar, perror_mensagem, ...) LOG_PCC("erro", _VERMELHO, encerrar, perror_mensagem, __VA_ARGS__)

#endif // LOG_H
