#include <stdio.h>
#include <string.h>
#include "plist.h"
#include "lexico/lexico.h"
#include "sintatico/gramatica.h"

int main(int argc, char const *argv[]) {
	pcc_gramatica_init();

	if (argc < 2) {
		LOG_PCC_ERRO(1, NULL, "missing filename");
	}

	pcc_gramatica_teste(argv[1]);

	return 0;
}
