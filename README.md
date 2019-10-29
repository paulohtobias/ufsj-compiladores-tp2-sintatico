# Paulo C Compiler: Analisador Sintático

**Segundo trabalho prático da disciplina de compiladores. Analisador Sintático.**

---
## Sobre

**Autor:**  Paulo Henrique Tobias

**Apresentado em:** 29/09/2019

Este analisador almeja fornecer 100% de suporte a caracteres UTF-8. Sendo assim, símbolos como `ç`, `à`, `と`, `俺` e `🤙` são todos considerados válidos para compor um identificador.

---

## Dependências

[libpcre2-8 - PCRE2 - Perl compatible regular expressions C library (2nd API) with 8 bit character support.](https://www.pcre.org/)

---
## Compilação

Comando: `make`

---
## Execução

Comando: `./pcc [OPCOES] ARQUIVO`

Onde `ARQUIVO` é o caminho de um código fonte da linguagem **C**.

Onde `OPCOES` pode ser zero ou mais das seguintes opções:
```
-std=PADRAO: Usa a gramática PADRAO
--print-gramatica NIVEL: Se > 0, exibe a gramática na tela.
                         Se > 1, encerra o programa sem realizar a análise.
-d NIVEL: Modo debug. Mostra a pilha e a entrada a cada passo da execução.
              0: Apenas mostra a pilha.
              1: Pausa a execução a cada passo.
-h | --help: Mostra esta mensagem de ajuda e encerra.
```

---
## Limitações

- Não suporta diretivas de pré-processamento.
- Não suporta o identificador de número variável de argumentos de função `...` visto nas famílias de funções `printf` e `scanf`.
- Suporte à operadores com associatividade à direita limitada.
- Novos tipos criados com typedef não são aceitos
- Statments dento de if e else só podem ser compostos, i.e., cercado por chaves `{` e `}`.
- Declaração foi drasticamente simplificada.
- Etc.


---
## Saída

Durante a análise do código, erros e warnings encontrados serão exibidos na saída de erros `stderr` no seguinte formato:

**`arquivo`:`linha`:`coluna`:** `mensagem`

    ...lexema...
       ^~~~~~

A análise gera uma lista com a ordem de uso das produções para uso futuro.

Ao final da execução, é exibido na tela a quantidade de erros e warnings.

---

## Padrão
Este programa aceita diferentes gramáticas através da opção `-std=PADRAO`. Se o arquivo `PADRAO.pccg` existir, então a gramática usada para fazer a análise do código será a descrita neste arquivo. No formato pccg (Paulo C Compiler Grammar), as produções são descritas como

```
NÃO-TERMINAL := [SIMBOLOS...]
```
Qualquer símbolo que não consta na lista de tokens (também chamados de terminais)
a seguir será considerado um não-terminal (também chamado de variável).

| token                     	|
|---------------------------	|
| identificador                 |
| auto                      	|
| break                     	|
| case                      	|
| char                      	|
| const                     	|
| continue                  	|
| default                   	|
| do                        	|
| double                    	|
| else                      	|
| enum                      	|
| extern                    	|
| float                     	|
| for                       	|
| goto                      	|
| if                        	|
| int                       	|
| long                      	|
| register                  	|
| return                    	|
| short                     	|
| signed                    	|
| sizeof                    	|
| static                    	|
| struct                    	|
| switch                    	|
| typedef                   	|
| union                     	|
| unsigned                  	|
| void                      	|
| volatile                  	|
| while                     	|
| string-literal            	|
| char-literal              	|
| int-literal               	|
| unsigned-int-literal      	|
| long-int-literal          	|
| unsigned-long-int-literal 	|
| float-literal             	|
| double-literal            	|
| long-double-literal       	|
| ->                        	|
| .                         	|
| !                         	|
| ~                         	|
| ++                        	|
| --                        	|
| *                         	|
| &                         	|
| /                         	|
| %                         	|
| +                         	|
| -                         	|
| <<                        	|
| >>                        	|
| <                         	|
| <=                        	|
| >                         	|
| >=                        	|
| ==                        	|
| !=                        	|
| ^                         	|
| \|                         	|
| &&                        	|
| \|\|                        	|
| ?                         	|
| =                         	|
| +=                        	|
| -=                        	|
| *=                        	|
| /=                        	|
| %=                        	|
| &=                        	|
| ^=                        	|
| \|=                        	|
| <<=                       	|
| >>=                       	|
| (                         	|
| )                         	|
| [                         	|
| ]                         	|
| {                         	|
| }                         	|
| ,                         	|
| :                         	|

---

## Exemplos
### Sucesso

Entrada: [teste-sucesso.c](https://github.com/paulohtobias/ufsj-compiladores-tp2-sintatico/blob/master/teste-sucesso.c)
```C
// Variáveis globais
int variável_global = -5;

typedef struct teste_sucesso {
	int a, b;
	float c;
	char n[10];
} teste_sucesso;


int funcao(int a, int b) {
	return a + b * variável_global;
}

int main() {
	int a = 2, é = 5;

	int res = funcao(a, é);

	if (res) {
		for (res; res >= 0; res--) {
			printf("res: %d\n", res);
		}
	} else {
		res += 30;
		while (res > 20) {
			res--;
		}

		do {
			res -= 2;
		} while (res);
	}

	printf("O resultado é: %d\n", res);

	return 0;
}
```

Saída: `./pcc teste-sucesso.c`
```
Fim
Warnings: 0
Erros: 0
```

---

### Erros e warnings

Entrada: [teste-erro-1.c](https://github.com/paulohtobias/ufsj-compiladores-tp2-sintatico/blob/master/teste-erro-1.c)
```C
// Variáveis globais
int variável_global = -5, v2 =, v3 = /;

struct teste_erro {
	int a, b;
	float c;
	char n[10]
} teste_erro, ts2;
```

Saída: `./pcc teste-erro-1.c`
```
teste-erro-1.c:2:31: erro: esperava string-literal, (, identificador, !, ~, ++, --, +, -, *, &, sizeof, char-literal, int-literal, unsigned-int-literal, long-int-literal, unsigned-long-int-literal, float-literal, double-literal ou long-double-literal antes do token ,
int variável_global = -5, v2 =, v3 = /;
                              ^
teste-erro-1.c:2:38: erro: esperava string-literal, (, identificador, !, ~, ++, --, +, -, *, &, sizeof, char-literal, int-literal, unsigned-int-literal, long-int-literal, unsigned-long-int-literal, float-literal, double-literal ou long-double-literal antes do token /
int variável_global = -5, v2 =, v3 = /;
                                     ^
teste-erro-1.c:8:1: erro: esperava , ou ; antes do token }
} teste_erro, ts2;
^
teste-erro-1.c:8:18: erro: esperava const, extern, static, void, char, short, int, long, float, double, signed, unsigned, struct ou } antes do fim inesperado de input
} teste_erro, ts2;
                 ^
Fim
Warnings: 0
Erros: 4
```

---

Entrada: [teste-erro-2.c](https://github.com/paulohtobias/ufsj-compiladores-tp2-sintatico/blob/master/teste-erro-2.c)
```C
int funcao(int a, int b) {
	return a + b * ;
}

int main() {
	int a = 2,  = 5;

	int res  funcao(a, é);

	if (res) {
		for (res; res >= 0; res--) {
			printf("res: %d\n", res);
		}
	} else {
		res += 30;
		while (res > 20) {
			res--;
		}

		do {
			res -= 2;
		}  (res);
	}

	printf("O resultado é: %d\n", res)

	return 0;
}
```

Saída: `./pcc teste-erro-2.c`
```
teste-erro-2.c:2:17: erro: esperava !, ~, ++, --, +, -, *, &, sizeof, char-literal, int-literal, unsigned-int-literal, long-int-literal, unsigned-long-int-literal, float-literal, double-literal, long-double-literal, string-literal, ( ou identificador antes do token ;
	return a + b * ;
	               ^
teste-erro-2.c:6:14: erro: esperava *, identificador ou ) antes do token =
	int a = 2,  = 5;
	            ^
teste-erro-2.c:8:11: erro: esperava =, [, , ou ; antes do token identificador
	int res  funcao(a, é);
	         ^~~~~~
teste-erro-2.c:22:6: erro: esperava while antes do token (
		}  (res);
		   ^
teste-erro-2.c:27:2: erro: esperava *, /, %, +, -, <<, >>, <, <=, >, >=, ==, !=, &, ^, |, ||, =, +=, -=, *=, /=, %=, &=, ^=, |=, <<=, >>=, ,, ;, ], && ou ) antes do token return
	return 0;
	^~~~~~
Fim
Warnings: 0
Erros: 5
```
