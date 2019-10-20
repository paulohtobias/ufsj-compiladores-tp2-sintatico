#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "plist.h"
#include "lexico/lexico.h"
#include "sintatico/gramatica.h"

int main(int argc, char const *argv[]) {

	pcc_gramatica_init();

	char vstr[][6] = {
		"L", "L'", "S"
	};

	for (size_t i = 0; i < plist_len(gramatica.producoes); i++) {
		const pcc_producao_t *producao = gramatica.producoes + i;

		printf("%2d: %-2s ->", producao->id, vstr[producao->origem]);

		if (plist_len(producao->simbolos) == 0) {
			printf(" Ɛ");
		} else {
			for (size_t j = 0; j < plist_len(producao->simbolos); j++) {
				const pcc_simbolo_t *simbolo = producao->simbolos + j;

				if (simbolo->tipo == SIMBOLO_TERMINAL) {
					token_t token;
					token.tipo = simbolo->id.token.tipo;
					token.subtipo = simbolo->id.token.subtipo;

					putchar(' ');
					for (const char *str = token_tipo_str(&token); *str != '\0'; str++) {
						putchar(tolower(*str));
					}
				} else {
					printf(" %s", vstr[simbolo->id.variavel]);
				}
			}
		}

		putchar('\n');
	}

	for (size_t i = 0; i < plist_len(gramatica.variaveis); i++) {
		const pcc_variavel_t *variavel = gramatica.variaveis + i;

		printf("%-2s:\n", vstr[variavel->cod]);

		printf("\tVazio: %s\n", variavel->gera_vazio ? "sim" : "não");

		printf("\tFIRST: ");
		for (size_t j = 0; j < plist_len(variavel->first); j++) {
			const pcc_simbolo_id_terminal_t *terminal = variavel->first + j;

			token_t token;
			token.tipo = terminal->tipo;
			token.subtipo = terminal->subtipo;
			putchar(' ');
			for (const char *str = token_tipo_str(&token); *str != '\0'; str++) {
				putchar(tolower(*str));
			}
		}

		printf("\n\tFOLLOW: ");
		for (size_t j = 0; j < plist_len(variavel->follow); j++) {
			const pcc_simbolo_id_terminal_t *terminal = variavel->follow + j;

			token_t token;
			token.tipo = terminal->tipo;
			token.subtipo = terminal->subtipo;
			putchar(' ');
			for (const char *str = token_tipo_str(&token); *str != '\0'; str++) {
				putchar(tolower(*str));
			}
		}
		puts("");
	}


	return 0;
}
