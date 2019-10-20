/**
 * PCC - Paulo C Compiler
 *
 * lexico/token/operador.h
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#ifndef LEXICO_TOKEN_OPERADOR_H
#define LEXICO_TOKEN_OPERADOR_H

// X-Macro pra cada tipo de operador.
#define SUBTIPOS \
	SUBTIPO(TK_OP_SETA, "->", "seta") \
	SUBTIPO(TK_OP_PONTO, ".", "ponto") \
	SUBTIPO(TK_OP_NOTL, "!", "notl") \
	SUBTIPO(TK_OP_NOTB, "~", "notb") \
	SUBTIPO(TK_OP_INC, "++", "inc") \
	SUBTIPO(TK_OP_DEC, "--", "dec") \
	SUBTIPO(TK_OP_AST, "*", "ast") \
	SUBTIPO(TK_OP_EC, "&", "ec") \
	SUBTIPO(TK_OP_BARRA, "/", "barra") \
	SUBTIPO(TK_OP_MOD, "%", "mod") \
	SUBTIPO(TK_OP_MAIS, "+", "mais") \
	SUBTIPO(TK_OP_MENOS, "-", "menos") \
	SUBTIPO(TK_OP_SHL, "<<", "shl") \
	SUBTIPO(TK_OP_SHR, ">>", "shr") \
	SUBTIPO(TK_OP_MENOR, "<", "menor") \
	SUBTIPO(TK_OP_MENOR_IGUAL, "<=", "menor_igual") \
	SUBTIPO(TK_OP_MAIOR, ">", "maior") \
	SUBTIPO(TK_OP_MAIOR_IGUAL, ">=", "maior_igual") \
	SUBTIPO(TK_OP_IGUAL, "==", "igual") \
	SUBTIPO(TK_OP_DIF, "!=", "dif") \
	SUBTIPO(TK_OP_XOR, "^", "xor") \
	SUBTIPO(TK_OP_ORB, "|", "orb") \
	SUBTIPO(TK_OP_EL, "&&", "el") \
	SUBTIPO(TK_OP_ORL, "||", "orl") \
	SUBTIPO(TK_OP_TER_C, "?", "ter_c") \
	SUBTIPO(TK_OP_ATRIB, "=", "atrib") \
	SUBTIPO(TK_OP_AUTO_INC, "+=", "auto_inc") \
	SUBTIPO(TK_OP_AUTO_DEC, "-=", "auto_dec") \
	SUBTIPO(TK_OP_AUTO_MUL, "*=", "auto_mul") \
	SUBTIPO(TK_OP_AUTO_DIV, "/=", "auto_div") \
	SUBTIPO(TK_OP_AUTO_MOD, "%=", "auto_mod") \
	SUBTIPO(TK_OP_AUTO_EB, "&=", "auto_eb") \
	SUBTIPO(TK_OP_AUTO_XORB, "^=", "auto_xorb") \
	SUBTIPO(TK_OP_AUTO_ORB, "|=", "auto_orb") \
	SUBTIPO(TK_OP_AUTO_SHL, "<<=", "auto_shl") \
	SUBTIPO(TK_OP_AUTO_SHR, ">>=", "auto_shr")


/// Tipos de operador.
#define SUBTIPO(cod, lex, str) cod,
enum {
	SUBTIPOS
};
#undef SUBTIPO

#endif // LEXICO_TOKEN_OPERADOR_H
