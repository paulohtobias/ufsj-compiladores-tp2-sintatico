#include <stdio.h>
#include <string.h>
#include "plist.h"
#include "lexico/lexico.h"
#include "sintatico/gramatica.h"

int main(int argc, char const *argv[]) {
	if (argc < 2) {
		LOG_PCC_ERRO(1, NULL, "missing filename");
	}

	pcc_gramatica_init(NULL);

	pcc_gramatica_teste(argv[1]);

	pcc_gramatica_finalizar();
	pcc_codigo_fonte_finalizar();

	return 0;
}
