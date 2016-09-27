#include <string.h>

#include "stringUtils.h"
#include "linha.h"

int encontraLinha(int enderecoMemoria, Linha* linhas, int linhasCount) {
	int i;

	if (linhas == NULL)
		return -1;

	for (i = 0; i < linhasCount; i++)
		if (linhas[i]->enderecoMemoria == enderecoMemoria)
			return i;

	return -1;
}

Linha novaLinha(int enderecoMemoria) {
	Linha linha = (Linha) malloc(sizeof(_linha));

	linha->enderecoMemoria = enderecoMemoria;
	linha->direita = NULL;
	linha->esquerda = NULL;
	linha->ehDado = false;

	return linha;
}

Comando novoComando(int instrucao, char* parametro) {
	Comando comando = (Comando) malloc(sizeof(_comando));

	comando->instrucao = instrucao;

	trim(parametro);
	if (ehVazia(parametro))
		comando->parametroStr = NULL;
	else if (ehNumerica(parametro)) {
		comando->parametro = strToInt(parametro);
		comando->parametroStr = NULL;
	} else {
		comando->parametroStr = (char *) malloc(sizeof(char) * (length(parametro) + 1));
		substring(comando->parametroStr, parametro, 0, length(parametro));
	}

	return comando;
}

bool adicionaComando(int enderecoMemoria, int dado, int instrucao, char* parametro, Linha** linhas, int* linhasCount, bool esquerda) {
	int ondeInserir;
	Linha* aux = *linhas;
	ondeInserir = encontraLinha(enderecoMemoria, *linhas, *linhasCount);

	/* se a linha ja fora preenchida */
	if (ondeInserir != -1 && (aux[ondeInserir]->ehDado ||
			(!esquerda && aux[ondeInserir]->direita != NULL)
			|| (esquerda && aux[ondeInserir]->esquerda != NULL)))
		return false;

	if (ondeInserir == -1) {
		ondeInserir = *linhasCount;
		*linhasCount = *linhasCount + 1;
		aux = (Linha *) realloc(aux, sizeof(Linha) * (*linhasCount  + 1));
		aux[ondeInserir] = novaLinha(enderecoMemoria);
	}

	if (instrucao == -1) {
		aux[ondeInserir]->dado = dado;
		aux[ondeInserir]->ehDado = true;
	} else if (esquerda)
		aux[ondeInserir]->esquerda = novoComando(instrucao, parametro);
	else
		aux[ondeInserir]->direita = novoComando(instrucao, parametro);

	*linhas = aux;

	return true;
}

void ordenarPorEnderecoDeMemoria(Linha** linhas, int linhasCount) {
	int i;
	int j;
	Linha* auxLinhas = *linhas;
	Linha aux;

	for (i = 0; i < linhasCount - 1; i++)
		for (j = i + 1; j < linhasCount; j++)
			if (auxLinhas[i]->enderecoMemoria > auxLinhas[j]->enderecoMemoria) {
				aux = auxLinhas[i];
				auxLinhas[i] = auxLinhas[j];
				auxLinhas[j] = aux;
			}
}

