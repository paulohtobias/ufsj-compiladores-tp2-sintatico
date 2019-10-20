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
#define TK_OP_SUBTIPOS \
	TK_OP_SUBTIPO(TK_OP_SETA, "->", "seta") \
	TK_OP_SUBTIPO(TK_OP_PONTO, ".", "ponto") \
	TK_OP_SUBTIPO(TK_OP_NOTL, "!", "notl") \
	TK_OP_SUBTIPO(TK_OP_NOTB, "~", "notb") \
	TK_OP_SUBTIPO(TK_OP_INC, "++", "inc") \
	TK_OP_SUBTIPO(TK_OP_DEC, "--", "dec") \
	TK_OP_SUBTIPO(TK_OP_AST, "*", "ast") \
	TK_OP_SUBTIPO(TK_OP_EC, "&", "ec") \
	TK_OP_SUBTIPO(TK_OP_BARRA, "/", "barra") \
	TK_OP_SUBTIPO(TK_OP_MOD, "%", "mod") \
	TK_OP_SUBTIPO(TK_OP_MAIS, "+", "mais") \
	TK_OP_SUBTIPO(TK_OP_MENOS, "-", "menos") \
	TK_OP_SUBTIPO(TK_OP_SHL, "<<", "shl") \
	TK_OP_SUBTIPO(TK_OP_SHR, ">>", "shr") \
	TK_OP_SUBTIPO(TK_OP_MENOR, "<", "menor") \
	TK_OP_SUBTIPO(TK_OP_MENOR_IGUAL, "<=", "menor_igual") \
	TK_OP_SUBTIPO(TK_OP_MAIOR, ">", "maior") \
	TK_OP_SUBTIPO(TK_OP_MAIOR_IGUAL, ">=", "maior_igual") \
	TK_OP_SUBTIPO(TK_OP_IGUAL, "==", "igual") \
	TK_OP_SUBTIPO(TK_OP_DIF, "!=", "dif") \
	TK_OP_SUBTIPO(TK_OP_XOR, "^", "xor") \
	TK_OP_SUBTIPO(TK_OP_ORB, "|", "orb") \
	TK_OP_SUBTIPO(TK_OP_EL, "&&", "el") \
	TK_OP_SUBTIPO(TK_OP_ORL, "||", "orl") \
	TK_OP_SUBTIPO(TK_OP_TER_C, "?", "ter_c") \
	TK_OP_SUBTIPO(TK_OP_ATRIB, "=", "atrib") \
	TK_OP_SUBTIPO(TK_OP_AUTO_INC, "+=", "auto_inc") \
	TK_OP_SUBTIPO(TK_OP_AUTO_DEC, "-=", "auto_dec") \
	TK_OP_SUBTIPO(TK_OP_AUTO_MUL, "*=", "auto_mul") \
	TK_OP_SUBTIPO(TK_OP_AUTO_DIV, "/=", "auto_div") \
	TK_OP_SUBTIPO(TK_OP_AUTO_MOD, "%=", "auto_mod") \
	TK_OP_SUBTIPO(TK_OP_AUTO_EB, "&=", "auto_eb") \
	TK_OP_SUBTIPO(TK_OP_AUTO_XORB, "^=", "auto_xorb") \
	TK_OP_SUBTIPO(TK_OP_AUTO_ORB, "|=", "auto_orb") \
	TK_OP_SUBTIPO(TK_OP_AUTO_SHL, "<<=", "auto_shl") \
	TK_OP_SUBTIPO(TK_OP_AUTO_SHR, ">>=", "auto_shr")


/// Tipos de operador.
#define TK_OP_SUBTIPO(cod, lex, str) cod,
enum {
	TK_OP_SUBTIPOS
};
#undef TK_OP_SUBTIPO

#endif // LEXICO_TOKEN_OPERADOR_H
