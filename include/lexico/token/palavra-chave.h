/**
 * PCC - Paulo C Compiler
 *
 * token/palavra-chave.h
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#ifndef TOKEN_PALAVRA_CHAVE_H
#define TOKEN_PALAVRA_CHAVE_H

#include "lexico/token.h"

// X-Macro pra cada palavra chave.
#define SUBTIPOS \
	SUBTIPO(TK_KW_AUTO, "auto") \
	SUBTIPO(TK_KW_BREAK, "break") \
	SUBTIPO(TK_KW_CASE, "case") \
	SUBTIPO(TK_KW_CHAR, "char") \
	SUBTIPO(TK_KW_CONST, "const") \
	SUBTIPO(TK_KW_CONTINUE, "continue") \
	SUBTIPO(TK_KW_DEFAULT, "default") \
	SUBTIPO(TK_KW_DO, "do") \
	SUBTIPO(TK_KW_DOUBLE, "double") \
	SUBTIPO(TK_KW_ELSE, "else") \
	SUBTIPO(TK_KW_ENUM, "enum") \
	SUBTIPO(TK_KW_EXTERN, "extern") \
	SUBTIPO(TK_KW_FLOAT, "float") \
	SUBTIPO(TK_KW_FOR, "for") \
	SUBTIPO(TK_KW_GOTO, "goto") \
	SUBTIPO(TK_KW_IF, "if") \
	SUBTIPO(TK_KW_INT, "int") \
	SUBTIPO(TK_KW_LONG, "long") \
	SUBTIPO(TK_KW_REGISTER, "register") \
	SUBTIPO(TK_KW_RETURN, "return") \
	SUBTIPO(TK_KW_SHORT, "short") \
	SUBTIPO(TK_KW_SIGNED, "signed") \
	SUBTIPO(TK_KW_SIZEOF, "sizeof") \
	SUBTIPO(TK_KW_STATIC, "static") \
	SUBTIPO(TK_KW_STRUCT, "struct") \
	SUBTIPO(TK_KW_SWITCH, "switch") \
	SUBTIPO(TK_KW_TYPEDEF, "typedef") \
	SUBTIPO(TK_KW_UNION, "union") \
	SUBTIPO(TK_KW_UNSIGNED, "unsigned") \
	SUBTIPO(TK_KW_VOID, "void") \
	SUBTIPO(TK_KW_VOLATILE, "volatile") \
	SUBTIPO(TK_KW_WHILE, "while")

/// Tipos de palavra-chave.
#define SUBTIPO(cod, str) cod,
enum {
	SUBTIPOS
};
#undef SUBTIPO


int token_palavra_chave_buscar(const token_t *token);

const char *token_palavra_chave_subtipo_str(uint32_t subtipo);

#endif // TOKEN_PALAVRA_CHAVE_H
