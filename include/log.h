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
#include "lexico/codigo_fonte.h"
#include "utils.h"

// Erros do código.

/// Contador de warnings.
extern uint32_t _log_warnings;

/// Contador de erros.
extern uint32_t _log_erros;

void log_mensagem(const char *tipo, const char *tipo_cor, uint32_t *log, const pcc_codigo_fonte_t *fonte, int32_t linha, int32_t coluna, size_t token_comprimento, ...);

#define _LOG(tipo, COR_TIPO, gvar, arquivo, linha, coluna, token_comprimento, ...) \
	log_mensagem(tipo, COR_NEGRITO(COR_TIPO), &(gvar), pcc_codigo_fonte_abir(arquivo), linha, coluna, token_comprimento, __VA_ARGS__)

#define LOG_WARNING(arquivo, linha, coluna, token_comprimento, ...) _LOG("warning", _AMARELO, _log_warnings, arquivo, linha, coluna, token_comprimento, __VA_ARGS__)
#define LOG_ERRO(arquivo, linha, coluna, token_comprimento, ...) _LOG("erro", _VERMELHO, _log_erros, arquivo, linha, coluna, token_comprimento, __VA_ARGS__)


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
