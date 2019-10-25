#include <stdio.h>
#include <string.h>
#include "plist.h"
#include "lexico/lexico.h"
#include "sintatico/gramatica.h"

int mostrar_tokens() {
	lexico_init();
	for (size_t i = 0; i < TK_COUNT; i++) {
		for (size_t j = 0; j < plist_len(__token_str[i]); j++) {
			const char *str = token_tipo_subtipo_str(i, j);
			uint32_t t, s;
			printf("%d, %2d: %s\n", (int) i, (int) j, str);
			token_str_tipo_subtipo(str, &t, &s);
			printf("%d, %2d: %s\n\n", t, s, str);
		}
		puts("");
	}
	lexico_finalizar();
	return 0;
}

int main(int argc, char const *argv[]) {
	//return mostrar_tokens();

	if (argc < 2) {
		LOG_PCC_ERRO(1, NULL, "missing filename");
	}

	pcc_gramatica_init();

	pcc_gramatica_teste(argv[1]);

	return 0;
}
