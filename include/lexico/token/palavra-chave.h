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
#define TK_KW_SUBTIPOS \
	TK_KW_SUBTIPO(TK_KW_AUTO, "auto") \
	TK_KW_SUBTIPO(TK_KW_BREAK, "break") \
	TK_KW_SUBTIPO(TK_KW_CASE, "case") \
	TK_KW_SUBTIPO(TK_KW_CHAR, "char") \
	TK_KW_SUBTIPO(TK_KW_CONST, "const") \
	TK_KW_SUBTIPO(TK_KW_CONTINUE, "continue") \
	TK_KW_SUBTIPO(TK_KW_DEFAULT, "default") \
	TK_KW_SUBTIPO(TK_KW_DO, "do") \
	TK_KW_SUBTIPO(TK_KW_DOUBLE, "double") \
	TK_KW_SUBTIPO(TK_KW_ELSE, "else") \
	TK_KW_SUBTIPO(TK_KW_ENUM, "enum") \
	TK_KW_SUBTIPO(TK_KW_EXTERN, "extern") \
	TK_KW_SUBTIPO(TK_KW_FLOAT, "float") \
	TK_KW_SUBTIPO(TK_KW_FOR, "for") \
	TK_KW_SUBTIPO(TK_KW_GOTO, "goto") \
	TK_KW_SUBTIPO(TK_KW_IF, "if") \
	TK_KW_SUBTIPO(TK_KW_INT, "int") \
	TK_KW_SUBTIPO(TK_KW_LONG, "long") \
	TK_KW_SUBTIPO(TK_KW_REGISTER, "register") \
	TK_KW_SUBTIPO(TK_KW_RETURN, "return") \
	TK_KW_SUBTIPO(TK_KW_SHORT, "short") \
	TK_KW_SUBTIPO(TK_KW_SIGNED, "signed") \
	TK_KW_SUBTIPO(TK_KW_SIZEOF, "sizeof") \
	TK_KW_SUBTIPO(TK_KW_STATIC, "static") \
	TK_KW_SUBTIPO(TK_KW_STRUCT, "struct") \
	TK_KW_SUBTIPO(TK_KW_SWITCH, "switch") \
	TK_KW_SUBTIPO(TK_KW_TYPEDEF, "typedef") \
	TK_KW_SUBTIPO(TK_KW_UNION, "union") \
	TK_KW_SUBTIPO(TK_KW_UNSIGNED, "unsigned") \
	TK_KW_SUBTIPO(TK_KW_VOID, "void") \
	TK_KW_SUBTIPO(TK_KW_VOLATILE, "volatile") \
	TK_KW_SUBTIPO(TK_KW_WHILE, "while")

/// Tipos de palavra-chave.
#define TK_KW_SUBTIPO(cod, str) cod,
enum {
	TK_KW_SUBTIPOS
};
#undef TK_KW_SUBTIPO


int token_palavra_chave_buscar(const token_t *token);

const char *token_palavra_chave_subtipo_str(int32_t subtipo);

#endif // TOKEN_PALAVRA_CHAVE_H
