/**
 * PCC - Paulo C Compiler
 *
 * gramatica.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <stdlib.h>
#include "plist.h"
#include "lexico/lexico.h"
#include "sintatico/gramatica.h"

pcc_ll1_t gramatica;

void pcc_gramatica_init() {
	// TODO
}

void pcc_gramatica_teste() {
	enum {
		PCC_D,
		PCC_T,
		PCC_IL
	};

	char vstr[][6] = {
		"D", "T", "IL"
	};

	pcc_ll1_init(&gramatica, 3);

	pcc_producao_t producoes[5] = {0};
	int i = 0;
	{
		producoes[i].origem = PCC_D;
		pcc_simbolo_t simbolos[] = {
			{
				SIMBOLO_VARIAVEL,
				{.variavel=PCC_T}
			},
			{
				SIMBOLO_VARIAVEL,
				{.variavel=PCC_IL}
			}
		};
		for (int j = 0; j < sizeof simbolos / sizeof simbolos[0]; j++) {
			plist_append(producoes[i].simbolos, simbolos[j]);
		}
		pcc_ll1_add_producao(&gramatica, producoes[i]);
		i++;
	}
	{
		producoes[i].origem = PCC_T;
		pcc_simbolo_t simbolos[] = {
			{
				SIMBOLO_TERMINAL,
				{.token={TK_KW, TK_KW_INT, "int"}}
			}
		};
		for (int j = 0; j < sizeof simbolos / sizeof simbolos[0]; j++) {
			plist_append(producoes[i].simbolos, simbolos[j]);
		}
		pcc_ll1_add_producao(&gramatica, producoes[i]);
		i++;
	}
	{
		producoes[i].origem = PCC_T;
		pcc_simbolo_t simbolos[] = {
			{
				SIMBOLO_TERMINAL,
				{.token={TK_KW, TK_KW_FLOAT, "float"}}
			}
		};
		for (int j = 0; j < sizeof simbolos / sizeof simbolos[0]; j++) {
			plist_append(producoes[i].simbolos, simbolos[j]);
		}
		pcc_ll1_add_producao(&gramatica, producoes[i]);
		i++;
	}
	{
		producoes[i].origem = PCC_IL;
		pcc_simbolo_t simbolos[] = {
			{
				SIMBOLO_TERMINAL,
				{.token={TK_ID, 0, "identificador"}}
			}
		};
		for (int j = 0; j < sizeof simbolos / sizeof simbolos[0]; j++) {
			plist_append(producoes[i].simbolos, simbolos[j]);
		}
		pcc_ll1_add_producao(&gramatica, producoes[i]);
		i++;
	}
	{
		producoes[i].origem = PCC_IL;
		pcc_simbolo_t simbolos[] = {
			{
				SIMBOLO_TERMINAL,
				{.token={TK_ID, 0, "identificador"}}
			},
			{
				SIMBOLO_TERMINAL,
				{.token={TK_EXT, TK_EXT_VIRGULA, ","}}
			},
			{
				SIMBOLO_VARIAVEL,
				{.variavel=PCC_IL}
			}
		};
		for (int j = 0; j < sizeof simbolos / sizeof simbolos[0]; j++) {
			plist_append(producoes[i].simbolos, simbolos[j]);
		}
		pcc_ll1_add_producao(&gramatica, producoes[i]);
		i++;
	}

	pcc_ll1_calcular(&gramatica);

	pcc_ll1_print(&gramatica, (char *) vstr, sizeof vstr[0]);
}

void pcc_gramatica_teste1() {
	// Teste

	enum {
		PCC_L,
		PCC_LL,
		PCC_S
	};

	pcc_ll1_init(&gramatica, 3);

	pcc_producao_t producoes[5] = {0};
	int i = 0;
	producoes[i].origem = PCC_L;
	pcc_simbolo_t simbolos11[] = {
		{
			SIMBOLO_VARIAVEL,
			{.variavel=PCC_S}
		},
		{
			SIMBOLO_VARIAVEL,
			{.variavel=PCC_LL}
		}
	};
	for (int j = 0; j < sizeof simbolos11 / sizeof simbolos11[0]; j++) {
		plist_append(producoes[i].simbolos, simbolos11[j]);
	}
	pcc_ll1_add_producao(&gramatica, producoes[i]);
	i++;

	producoes[i].origem = PCC_LL;
	pcc_simbolo_t simbolos12[] = {
		{
			SIMBOLO_TERMINAL,
			{.token={TK_EXT, TK_EXT_PT_VIRGULA}}
		},
		{
			SIMBOLO_VARIAVEL,
			{.variavel=PCC_LL}
		},
		{
			SIMBOLO_VARIAVEL,
			{.variavel=PCC_S}
		}
	};
	for (int j = 0; j < sizeof simbolos12 / sizeof simbolos12[0]; j++) {
		plist_append(producoes[i].simbolos, simbolos12[j]);
	}
	pcc_ll1_add_producao(&gramatica, producoes[i]);
	i++;

	producoes[i].origem = PCC_LL;
	producoes[i].simbolos = NULL;
	pcc_ll1_add_producao(&gramatica, producoes[i]);
	i++;

	producoes[i].origem = PCC_S;
	pcc_simbolo_t simbolos14[] = {
		{
			SIMBOLO_TERMINAL,
			{.token={TK_KW, TK_KW_RETURN}}
		}
	};
	for (int j = 0; j < sizeof simbolos14 / sizeof simbolos14[0]; j++) {
		plist_append(producoes[i].simbolos, simbolos14[j]);
	}
	pcc_ll1_add_producao(&gramatica, producoes[i]);
	i++;

	pcc_ll1_calcular(&gramatica);

	char vstr[][6] = {
		"L", "L'", "S"
	};
	pcc_ll1_print(&gramatica, (char *) vstr, sizeof vstr[0]);
}
