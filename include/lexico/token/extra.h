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
#define TK_EXT_SUBTIPOS \
	TK_EXT_SUBTIPO(TK_EXT_PAR_A, "(", "parenteses-abrir") \
	TK_EXT_SUBTIPO(TK_EXT_PAR_F, ")", "parenteses-fechar") \
	TK_EXT_SUBTIPO(TK_EXT_COL_A, "[", "colchete-abrir") \
	TK_EXT_SUBTIPO(TK_EXT_COL_F, "]", "colchete-fechar") \
	TK_EXT_SUBTIPO(TK_EXT_CHA_A, "{", "chave-abrir") \
	TK_EXT_SUBTIPO(TK_EXT_CHA_F, "}", "chave-fechar") \
	TK_EXT_SUBTIPO(TK_EXT_VIRGULA, ",", "virgula") \
	TK_EXT_SUBTIPO(TK_EXT_DOIS_PONTOS, ":", "dois-pontos") \
	TK_EXT_SUBTIPO(TK_EXT_PT_VIRGULA, ";", "ponto-virgula")

/// Tipos de extra.
#define TK_EXT_SUBTIPO(cod, lex, str) cod,
enum {
	TK_EXT_SUBTIPOS
};
#undef TK_EXT_SUBTIPO

#endif // LEXICO_TOKEN_EXTRA_H
