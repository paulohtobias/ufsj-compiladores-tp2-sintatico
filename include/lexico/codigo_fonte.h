/**
 * PCC - Paulo C Compiler
 *
 * lexico/codigo_fonte.h
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#ifndef LEXICO_CODIGO_FONTE_H
#define LEXICO_CODIGO_FONTE_H

#include <stdint.h>

typedef struct pcc_codigo_fonte_t {
	char *caminho;

	char *src;
	size_t src_tamanho;

	size_t *offset_linhas;
	int32_t linhas_qtd;
} pcc_codigo_fonte_t;

void pcc_codigo_fonte_init();

void pcc_codigo_fonte_finalizar();

/**
 * Retorna o código fonte do arquivo como string.
 * Se o arquivo não for encontrado, o arquivo será aberto para leitura.
 * Se o arquivo não existir, o programa é encerrado com status de erro.
 */
pcc_codigo_fonte_t *pcc_codigo_fonte_abir(const char *nome_arquivo);

/**
 * Retorna a linha do arquivo com possível deslocamento.
 *
 * linha e coluna começam a partir de 1.
 *
 * linha deve ser >= 1.
 * coluna pode ser < 1. Neste caso, a linha toda será retornada.
 */
char *pcc_codigo_fonte_get_linha(const pcc_codigo_fonte_t *fonte, int32_t linha, int32_t coluna);

/**
 * Libera a memória alocada para o código fonte.
 */
void pcc_codigo_fonte_liberar(pcc_codigo_fonte_t *fonte);

#endif // LEXICO_CODIGO_FONTE_H
