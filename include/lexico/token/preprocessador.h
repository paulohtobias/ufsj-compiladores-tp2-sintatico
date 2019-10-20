/**
 * PCC - Paulo C Compiler
 *
 * lexico/token/preprocessador.h
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#ifndef LEXICO_TOKEN_PREPROCESSADOR_H
#define LEXICO_TOKEN_PREPROCESSADOR_H

// X-Macro pra cada tipo de pre-processador.
#define SUBTIPOS \
	SUBTIPO(TK_PP_DEFINE, define) \
	SUBTIPO(TK_PP_UNDEF, undef) \
	SUBTIPO(TK_PP_INCLUDE, include) \
	SUBTIPO(TK_PP_LINE, line) \
	SUBTIPO(TK_PP_ERROR, error) \
	SUBTIPO(TK_PP_PRAGMA, pragma) \
	SUBTIPO(TK_PP_IF, if) \
	SUBTIPO(TK_PP_IFDEF, ifdef) \
	SUBTIPO(TK_PP_IFNDEF, ifndef) \
	SUBTIPO(TK_PP_ELIF, elif) \
	SUBTIPO(TK_PP_ELSE, else) \
	SUBTIPO(TK_PP_ENDIF, endif)

/// Tipos de pre-processador.
#define SUBTIPO(cod, nome) cod,
enum {
	SUBTIPOS
};
#undef SUBTIPO

#endif // LEXICO_TOKEN_PREPROCESSADOR_H
