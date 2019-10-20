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
#define SUBTIPOS \
	SUBTIPO(TK_CNST_STR, str, "string-literal") \
	SUBTIPO(TK_CNST_CHAR, char, "char") \
	SUBTIPO(TK_CNST_INT, int, "inteiro") \
	SUBTIPO(TK_CNST_UINT, uint, "inteiro-unsigned") \
	SUBTIPO(TK_CNST_LINT, lint, "inteiro-long") \
	SUBTIPO(TK_CNST_ULINT, ulint, "inteiro-unsigned_long") \
	SUBTIPO(TK_CNST_FLT, float, "float") \
	SUBTIPO(TK_CNST_DBL, double, "double") \
	SUBTIPO(TK_CNST_LDBL, ldouble, "double-long")

/// Tipos de constante.
#define SUBTIPO(cod, nome, str) cod,
enum {
	SUBTIPOS
};
#undef SUBTIPO

#endif // LEXICO_TOKEN_CONSTANTE_H
