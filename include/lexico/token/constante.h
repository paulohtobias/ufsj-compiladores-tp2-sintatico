/**
 * PCC - Paulo C Compiler
 *
 * lexico/token/constante.h
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#ifndef LEXICO_TOKEN_CONSTANTE_H
#define LEXICO_TOKEN_CONSTANTE_H

// X-Macro pra cada tipo de constante.
#define TK_CNST_SUBTIPOS \
	TK_CNST_SUBTIPO(TK_CNST_STR, str, "string-literal") \
	TK_CNST_SUBTIPO(TK_CNST_CHAR, char, "char") \
	TK_CNST_SUBTIPO(TK_CNST_INT, int, "inteiro") \
	TK_CNST_SUBTIPO(TK_CNST_UINT, uint, "inteiro-unsigned") \
	TK_CNST_SUBTIPO(TK_CNST_LINT, lint, "inteiro-long") \
	TK_CNST_SUBTIPO(TK_CNST_ULINT, ulint, "inteiro-unsigned_long") \
	TK_CNST_SUBTIPO(TK_CNST_FLT, float, "float") \
	TK_CNST_SUBTIPO(TK_CNST_DBL, double, "double") \
	TK_CNST_SUBTIPO(TK_CNST_LDBL, ldouble, "double-long")

/// Tipos de constante.
#define TK_CNST_SUBTIPO(cod, nome, str) cod,
enum {
	TK_CNST_SUBTIPOS
};
#undef TK_CNST_SUBTIPO

#endif // LEXICO_TOKEN_CONSTANTE_H
