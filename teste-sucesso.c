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
