/**
 * PCC - Paulo C Compiler
 *
 * token/constante.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <float.h>
#include <errno.h>
#include "lexico/token.h"
#include "lexico/token/constante.h"
#include "utils.h"
#include "plist.h"

/// Tipos de palavra-chave em string.
#define TK_CNST_SUBTIPO(cod, nome, str) str,
const char __constantes[][32] = {
	TK_CNST_SUBTIPOS
};
#undef TK_CNST_SUBTIPO
size_t __constantes_quantidade = ARR_TAMANHO(__constantes);

/// Funções init.
#define TK_CNST_SUBTIPO(cod, nome, str) static int nome ## _init(afd_t *afd);
TK_CNST_SUBTIPOS
#undef TK_CNST_SUBTIPO
static int number_init(afd_t *afd);

/// Funções adicionar.
static void str_adicionar(const void *contexto);
static void char_adicionar(const void *contexto);
static void int_adicionar(const void *contexto);
static void double_adicionar(const void *contexto);

/// Funções to_str
static char *str_to_str(const void *dados, size_t comprimento);
static char *char_to_str(const void *dados, size_t comprimento);
static char *int_to_str(const void *dados, size_t comprimento);
static char *double_to_str(const void *dados, size_t comprimento);

/// Funções de erro.
static void str_incompleta(const void *contexto);
static void char_incompleto(const void *contexto);

/// Outras funções.
static const char *subtipo_str(int32_t subtipo);
static char parse_escape_sequence(const token_t *token, const char **src);
static char *int_to_str(const void *dados, size_t comprimento);


/// init
int token_constante_init(afd_t *afd) {
	int res;

	#define TK_CNST_SUBTIPO(cod, nome, str) \
		if ((res = nome ## _init(afd)) != 0) { \
			return 1; \
		}

	TK_CNST_SUBTIPOS
	#undef TK_CNST_SUBTIPO

	if ((res = number_init(afd)) != 0) {
		return 1;
	}

	// Inicializando a tabela de símbolos de identificador.
	plist_create(tabela_simbolos[TK_CNST], __constantes_quantidade);
	memset(tabela_simbolos[TK_CNST], 0, __constantes_quantidade * sizeof *tabela_simbolos[TK_CNST]);

	// Inicializando a matriz de descrição.
	#define TK_CNST_SUBTIPO(cod, nome, str) \
		plist_append(__token_str[TK_CNST], str);

	TK_CNST_SUBTIPOS
	#undef TK_CNST_SUBTIPO

	return 0;
}
static int str_init(afd_t *afd) {
	int res;

	afd_t afd_str;
	afd_init(&afd_str, 4);

	// Estado inicial. Deve começar com aspas duplas.
	afd_transicao_t transicoes_0[] = {
		{1, {"\"", "\"", NULL}}
	};
	afd_str.estados[0] = afd_criar_estado(transicoes_0, ARR_TAMANHO(transicoes_0), false, NULL);

	// Estado 1.
	afd_transicao_t transicoes_1[] = {
		{1, {"[^\"\\\\\\n]", "[^\"\\\\\\n]", NULL}},
		{2, {"\\", "\\\\", NULL}},
		{3, {"\"", "\"", NULL}}
	};
	plist_append(
		afd_str.estados,
		afd_criar_estado(transicoes_1, ARR_TAMANHO(transicoes_1), false, str_incompleta)
	);

	// Estado 2.
	afd_transicao_t transicoes_2[] = {
		{1, {".", "."}}
	};
	plist_append(
		afd_str.estados,
		afd_criar_estado(transicoes_2, ARR_TAMANHO(transicoes_2), false, str_incompleta)
	);

	// Estado Final.
	plist_append(
		afd_str.estados,
		afd_criar_estado(NULL, 0, true, str_adicionar)
	);

	if ((res = afd_mesclar_automatos(afd, &afd_str)) != AFD_OK) {
		LOG_PCC_ERRO(0, NULL, "%s: Não foi possível iniciar: %s\n", __FUNCTION__, afd_get_erro(res));
	}

	afd_liberar(&afd_str);

	return res;
}
static int char_init(afd_t *afd) {
	int res;

	afd_t afd_char;
	afd_init(&afd_char, 4);

	// Estado inicial. Deve começar com aspas simples.
	afd_transicao_t transicoes_0[] = {
		{1, {"'", "'", NULL}}
	};
	afd_char.estados[0] = afd_criar_estado(transicoes_0, ARR_TAMANHO(transicoes_0), false, NULL);

	// Estado 1.
	afd_transicao_t transicoes_1[] = {
		{1, {"[^\'\\\\\\n]", "[^\'\\\\\\n]", NULL}},
		{2, {"\\", "\\\\", NULL}},
		{3, {"\'", "\'", NULL}}
	};
	plist_append(
		afd_char.estados,
		afd_criar_estado(transicoes_1, ARR_TAMANHO(transicoes_1), false, char_incompleto)
	);

	// Estado 2.
	afd_transicao_t transicoes_2[] = {
		{1, {".", "."}}
	};
	plist_append(
		afd_char.estados,
		afd_criar_estado(transicoes_2, ARR_TAMANHO(transicoes_2), false, char_incompleto)
	);

	// Estado Final.
	plist_append(
		afd_char.estados,
		afd_criar_estado(NULL, 0, true, char_adicionar)
	);

	if ((res = afd_mesclar_automatos(afd, &afd_char)) != AFD_OK) {
		fprintf(stderr, "%s: %s.\n Não foi possível iniciar.\n", __FUNCTION__, afd_get_erro(res));
	}

	afd_liberar(&afd_char);

	return res;
}
static int int_init(afd_t *afd) {
	return 0;
}
static int uint_init(afd_t *afd) {
	return 0;
}
static int lint_init(afd_t *afd) {
	return 0;
}
static int ulint_init(afd_t *afd) {
	return 0;
}
static int float_init(afd_t *afd) {
	return 0;
}
static int double_init(afd_t *afd) {
	return 0;
}
static int ldouble_init(afd_t *afd) {
	return 0;
}
static int number_init(afd_t *afd) {
	int res = 0;

	afd_t afd_number;
	afd_init(&afd_number, 6);

	// Estado 0.
	afd_transicao_t transicoes_0[] = {
		{1, {"\\d", "\\d", NULL}},
		{3, {"\\.", "\\.", NULL}}
	};
	afd_number.estados[0] = afd_criar_estado(transicoes_0, ARR_TAMANHO(transicoes_0), false, NULL);

	// Estado 1. Int
	afd_transicao_t transicoes_1[] = {
		{1, {"\\d", "\\d", NULL}},
		{5, {"[eE]", "[eE]", NULL}}, // É importante que esta transição venha antes da próxima.
		{2, {"[" regex_unicode_letter "]", "[" regex_unicode_letter "]", NULL}},
		{4, {"\\.", "\\.", NULL}},
	};
	plist_append(
		afd_number.estados,
		afd_criar_estado(transicoes_1, ARR_TAMANHO(transicoes_1), true, int_adicionar)
	);

	// Estado 2. Unsigned e/ou Long
	afd_transicao_t transicoes_2[] = {
		{2, {"\\w", "[\\d" regex_unicode_letter "]", NULL}},
	};
	plist_append(
		afd_number.estados,
		afd_criar_estado(transicoes_2, ARR_TAMANHO(transicoes_2), true, int_adicionar)
	);

	// Estado 3. Operador . para membro de struct.
	afd_transicao_t transicoes_3[] = {
		{4, {"\\d", "\\d", NULL}},
	};
	plist_append(
		afd_number.estados,
		afd_criar_estado(transicoes_3, ARR_TAMANHO(transicoes_3), false, NULL)
	);

	// Estado 4. Double
	afd_transicao_t transicoes_4[] = {
		{4, {"\\d", "\\d", NULL}},
		{5, {"[" regex_unicode_letter "]", "[" regex_unicode_letter "]", NULL}},
	};
	plist_append(
		afd_number.estados,
		afd_criar_estado(transicoes_4, ARR_TAMANHO(transicoes_4), true, double_adicionar)
	);

	// Estado 5. Notação exponencial e/ou Float ou Long Double
	afd_transicao_t transicoes_5[] = {
		{4, {"\\d", "\\d", NULL}},
		{5, {"[" regex_unicode_letter "]", "[" regex_unicode_letter "]", NULL}},
	};
	plist_append(
		afd_number.estados,
		afd_criar_estado(transicoes_5, ARR_TAMANHO(transicoes_5), true, double_adicionar)
	);

	if ((res = afd_mesclar_automatos(afd, &afd_number)) != AFD_OK) {
		LOG_PCC_ERRO(0, NULL, "%s: Não foi possível iniciar: %s\n", __FUNCTION__, afd_get_erro(res));
	}

	afd_liberar(&afd_number);

	return res;
}

/// adicionar
static void str_adicionar(const void *contexto) {
	token_t token = token_criar(TK_CNST, TK_CNST_STR, contexto);
	token.subtipo_to_str = subtipo_str;

	token.valor.tamanho = token.contexto.lexema_comprimento - 1;
	PMALLOC(token.valor.dados, token.valor.tamanho);
	token.valor.to_str = str_to_str;

	// Ignorando as aspas iniciais.
	const char *lexema = token.contexto._lexema + 1;
	size_t i;
	for (i = 0; *lexema != '\"'; i++) {
		if (*lexema == '\\') {
			((char *) token.valor.dados)[i] = parse_escape_sequence(&token, &lexema);
		} else {
			((char *) token.valor.dados)[i] = *lexema;
		}

		lexema++;
	}
	((char *) token.valor.dados)[i++] = '\0';

	if (i < token.valor.tamanho) {
		token.valor.tamanho = i;
		PREALLOC(token.valor.dados, token.valor.tamanho);
	}

	token_adicionar(&token);
}
static void char_adicionar(const void *contexto) {
	token_t token = token_criar(TK_CNST, TK_CNST_CHAR, contexto);
	token.subtipo_to_str = subtipo_str;

	token.valor.tamanho = 1; // sizeof(char) é sempre 1.
	PMALLOC(token.valor.dados, 1);
	token.valor.to_str = char_to_str;

	// Ignorando as aspas iniciais.
	const char *lexema = token.contexto._lexema + 1;
	if (*lexema == '\\') {
		*((char *) token.valor.dados) = parse_escape_sequence(&token, &lexema);
	} else if (*lexema == '\'') {
		*((char *) token.valor.dados) = '\0';
		pcc_log_erro(contexto, "constante char vazia");
		lexema--;
	} else {
		*((char *) token.valor.dados) = *lexema;
	}
	lexema++;

	// Checa se não existem mais caracteres.
	if (*lexema != '\'') {
		pcc_log_warning(contexto, "char com mais de um caractere");
	}

	token_adicionar(&token);
}
static void int_adicionar(const void *contexto) {
	token_t token = token_criar(TK_CNST, TK_CNST_INT, contexto);
	token.subtipo_to_str = subtipo_str;

	// Convertendo o lexema para inteiro.
	char *fim = NULL;
	uintmax_t valor = strtoumax(token.contexto._lexema, &fim, 0);
	size_t tamanho = sizeof valor;

	// Verificando se o sufixo é válido.
	bool u = false, l = false;
	bool sufixo_valido = false;
	size_t sufixo_offset = fim - token.contexto._lexema;
	while (sufixo_offset < token.contexto.lexema_comprimento) {
		char sufixo = tolower((unsigned char) token.contexto._lexema[sufixo_offset]);

		sufixo_valido = false;
		if (!u && sufixo == 'u') {
			sufixo_offset++;
			u = sufixo_valido = true;

			// Alterando o subtipo.
			token.subtipo = l ? TK_CNST_ULINT : TK_CNST_UINT;
		}
		if (!l && sufixo == 'l') {
			sufixo_offset++;
			l = sufixo_valido = true;

			// Alterando o subtipo.
			token.subtipo = u ? TK_CNST_ULINT : TK_CNST_LINT;
		}

		if (!sufixo_valido) {
			pcc_log_erro(contexto, "sufixo \"%.*s\" inválido em constante inteiro", token.contexto.lexema_comprimento - sufixo_offset, &token.contexto._lexema[sufixo_offset]);
			token_liberar(&token);
			return;
		}
	}

	// Verificando se houve overflow.
	if (valor == UINTMAX_MAX && errno == ERANGE) {
		pcc_log_warning(contexto, "constante inteira é grande demais");
	}

	// TODO: verificar se cabe em um int para reduzir tamanho.

	token.valor.tamanho = tamanho;
	PMALLOC(token.valor.dados, token.valor.tamanho);
	memcpy(token.valor.dados, &valor, sizeof valor);
	token.valor.to_str = int_to_str;

	token_adicionar(&token);
}
static void double_adicionar(const void *contexto) {
	token_t token = token_criar(TK_CNST, TK_CNST_DBL, contexto);
	token.subtipo_to_str = subtipo_str;

	// Convertendo o lexema para double.
	errno = 0;
	char *fim = NULL;
	long double valor = strtold(token.contexto._lexema, &fim);
	size_t tamanho = sizeof valor;

	// Verificando se o sufixo é válido.
	bool f = false, l = false;
	bool sufixo_valido = false;
	size_t sufixo_offset = fim - token.contexto._lexema;
	while (sufixo_offset < token.contexto.lexema_comprimento) {
		char sufixo = tolower((unsigned char) token.contexto._lexema[sufixo_offset]);

		sufixo_valido = false;
		if (!f && sufixo == 'f') {
			sufixo_offset++;
			f = sufixo_valido = true;

			// Alterando o subtipo.
			token.subtipo = TK_CNST_FLT;
		}
		if (!l && sufixo == 'l') {
			sufixo_offset++;
			l = sufixo_valido = true;

			// Alterando o subtipo.
			token.subtipo = TK_CNST_LDBL;
		}

		if (!sufixo_valido || (f && l)) {
			pcc_log_erro(contexto, "sufixo \"%.*s\" inválido em constante de ponto flutuante", token.contexto.lexema_comprimento - sufixo_offset, &token.contexto._lexema[sufixo_offset]);
			token_liberar(&token);
			return;
		}
	}

	// Verificando se houve overflow.
	if (errno == ERANGE) {
		if (valor == 0) {
			pcc_log_warning(contexto, "constante de ponto flutuante truncada para 0");
		} else {
			pcc_log_warning(contexto, "constante de ponto flutuante excede o alcance de '%s'", subtipo_str(token.subtipo));
		}
	}

	token.valor.tamanho = tamanho;
	PMALLOC(token.valor.dados, token.valor.tamanho);
	memcpy(token.valor.dados, &valor, sizeof valor);
	token.valor.to_str = double_to_str;

	token_adicionar(&token);
}

/// to_str
static char *str_to_str(const void *dados, size_t comprimento) {
	return strdup(dados);
}
static char *char_to_str(const void *dados, size_t comprimento) {
	char *str = malloc(2);

	str[0] = *((char *) dados);
	str[1] = '\0';

	return str;
}
static char *int_to_str(const void *dados, size_t comprimento) {
	char *str = malloc(64);

	uintmax_t valor;
	memcpy(&valor, dados, sizeof valor);

	sprintf(str, "%"PRIuMAX, valor);

	return str;
}
static char *double_to_str(const void *dados, size_t comprimento) {
	char *str = malloc(64);

	long double valor;
	memcpy(&valor, dados, sizeof valor);

	sprintf(str, "%Lf", valor);

	return str;
}

static void incompleto(char simbolo, const void *contexto) {
	pcc_log_erro(contexto, "caractere %c final ausente", simbolo);
}
static void str_incompleta(const void *contexto) {
	incompleto('"', contexto);
}
static void char_incompleto(const void *contexto) {
	incompleto('\'', contexto);
}


static const char *subtipo_str(int32_t subtipo) {
	if (subtipo < __constantes_quantidade) {
		return __constantes[subtipo];
	}

	return "Erro: subtipo de constante inválido";
}

static char parse_escape_sequence(const token_t *token, const char **src) {
	static char escape_table[][2] = {
		{'n', '\n'},
		{'t', '\t'},
		{'v', '\v'},
		{'b', '\b'},
		{'r', '\r'},
		{'f', '\f'},
		{'a', '\a'},
		{'\\', '\\'},
		{'?', '\?'},
		{'\'', '\''},
		{'\"', '\"'}
	};
	static const size_t escape_table_tamanho = ARR_TAMANHO(escape_table);

	(*src)++;
	// Procura na tabela pelos símbolos.
	for (int i = 0; i < escape_table_tamanho; i++) {
		if (escape_table[i][0] == **src) {
			return escape_table[i][1];
		}
	}

	// Identifica numerais.
	int base = -1;
	if (isdigit((unsigned char) **src)) {
		base = 8;
	} else if (**src == 'x') {
		(*src)++;
		base = 16;
	}

	if (base == -1) {
		pcc_log_warning(&token->contexto, "sequência de escape desconhecida: '\\%c'", **src);
		return **src;
	}

	char *end = NULL;
	long c = strtol(*src, &end, base);

	if (c > UCHAR_MAX) {
		pcc_log_warning(&token->contexto, "sequência de escape %s fora de alcance", base == 8 ? "octal": "hexadecimal");
	}

	*src = end - 1;

	return (char) c;
}
