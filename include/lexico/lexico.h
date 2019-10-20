/**
 * PCC - Paulo C Compiler
 *
 * lexico.h
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#ifndef LEXICO_H
#define LEXICO_H

#include "lexico/token.h"
#include "lexico/token/palavra-chave.h"
#include "lexico/token/constante.h"
#include "lexico/token/operador.h"
#include "lexico/token/extra.h"
#include "lexico/token/preprocessador.h"


int lexico_init();

void lexico_finalizar();

int lexico_parse(const char *src);

#endif // LEXICO_H
