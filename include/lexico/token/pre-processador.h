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
#define TK_PP_SUBTIPOS \
	TK_PP_SUBTIPO(TK_PP_DEFINE, define) \
	TK_PP_SUBTIPO(TK_PP_UNDEF, undef) \
	TK_PP_SUBTIPO(TK_PP_INCLUDE, include) \
	TK_PP_SUBTIPO(TK_PP_LINE, line) \
	TK_PP_SUBTIPO(TK_PP_ERROR, error) \
	TK_PP_SUBTIPO(TK_PP_PRAGMA, pragma) \
	TK_PP_SUBTIPO(TK_PP_IF, if) \
	TK_PP_SUBTIPO(TK_PP_IFDEF, ifdef) \
	TK_PP_SUBTIPO(TK_PP_IFNDEF, ifndef) \
	TK_PP_SUBTIPO(TK_PP_ELIF, elif) \
	TK_PP_SUBTIPO(TK_PP_ELSE, else) \
	TK_PP_SUBTIPO(TK_PP_ENDIF, endif)

/// Tipos de pre-processador.
#define TK_PP_SUBTIPO(cod, nome) cod,
enum {
	TK_PP_SUBTIPOS
};
#undef TK_PP_SUBTIPO

#endif // LEXICO_TOKEN_PREPROCESSADOR_H
