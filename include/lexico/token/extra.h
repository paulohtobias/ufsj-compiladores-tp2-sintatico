/**
 * PCC - Paulo C Compiler
 *
 * lexico/token/extra.h
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#ifndef LEXICO_TOKEN_EXTRA_H
#define LEXICO_TOKEN_EXTRA_H

// X-Macro pra cada tipo de extra.
#define SUBTIPOS \
	SUBTIPO(TK_EXT_PAR_A, "(", "parenteses-abrir") \
	SUBTIPO(TK_EXT_PAR_F, ")", "parenteses-fechar") \
	SUBTIPO(TK_EXT_COL_A, "[", "colchete-abrir") \
	SUBTIPO(TK_EXT_COL_F, "]", "colchete-fechar") \
	SUBTIPO(TK_EXT_CHA_A, "{", "chave-abrir") \
	SUBTIPO(TK_EXT_CHA_F, "}", "chave-fechar") \
	SUBTIPO(TK_EXT_VIRGULA, ",", "virgula") \
	SUBTIPO(TK_EXT_DOIS_PONTOS, ":", "dois-pontos") \
	SUBTIPO(TK_EXT_PT_VIRGULA, ";", "ponto-virgula")

/// Tipos de extra.
#define SUBTIPO(cod, lex, str) cod,
enum {
	SUBTIPOS
};
#undef SUBTIPO

#endif // LEXICO_TOKEN_EXTRA_H
