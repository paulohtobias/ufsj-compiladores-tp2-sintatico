#include <stdio.h>
#include <string.h>
#include "sintatico/gramatica.h"

int main(int argc, char const *argv[]) {
	const char *padrao = NULL;
	const char *fonte_caminho = NULL;

	// Pegando os argumentos.
	int argi;
	for (argi = 1; argi < argc; argi++) {
		if (argv[argi][0] != '-') {
			fonte_caminho = argv[argi];
		} else {
			const char *arg = argv[argi] + 1;
			if (strncmp(arg, "std=", 4) == 0) {
				padrao = arg + 4;
			} else if (strcmp(arg, "-print-gramatica") == 0) {
				argi++;
				if (argi < argc) {
					pcc_gramatica_g_print_gramatica = atoi(argv[argi]);
				} else {
					LOG_PCC_ERRO(1, NULL, "--print-gramatica precisa de um nível");
				}
			} else if (strcmp(arg, "d") == 0) {
				argi++;
				if (argi < argc) {
					pcc_ll1_g_debug = atoi(argv[argi]);
				} else {
					LOG_PCC_ERRO(1, NULL, "-d precisa de um nível");
				}
			} else if (strcmp(arg, "h") == 0 || strcmp(arg, "-help") == 0) {
				fprintf(stdout, "uso: %s [OPCOES] ARQUIVO\n", argv[0]);
				puts(
					"-std=PADRAO: Usa a gramática PADRAO\n\n"
					"--print-gramatica NIVEL: Se > 0, exibe a gramática na tela.\n"
					"                         Se > 1, encerra o programa sem realizar a análise.\n"
					"-d NIVEL: Modo debug. Mostra a pilha e a entrada a cada passo da execução.\n"
					"              0: Apenas mostra a pilha.\n"
					"              1: Pausa a execução a cada passo.\n"
					"-h | --help: Mostra esta mensagem de ajuda e encerra.\n\n"
					"ARQUIVO caminho para um código fonte da linguagem C.\n"
				);
				exit(0);
			} else {
				LOG_PCC_ERRO(
					1, NULL,
					"opção %s inválida\n\n"
					"uso: %s [OPCOES] ARQUIVO\n",
					argv[argi], argv[0]
				);
			}
		}
	}

	if (fonte_caminho == NULL) {
		LOG_PCC_ERRO(
			1, NULL,
			"faltando nome de arquivo\n\n"
			"uso: %s [OPCOES] ARQUIVO\n",
			argv[0]
		);
	}

	pcc_gramatica_init(padrao);

	pcc_gramatica_teste(fonte_caminho);

	pcc_gramatica_finalizar();
	pcc_codigo_fonte_finalizar();

	return 0;
}
