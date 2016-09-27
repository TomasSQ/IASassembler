#include <string.h>

#include "stringUtils.h"
#include "rotulo.h"
#include "instrucao.h"

bool comandoInvalido(char* comando) {
	int i;
	int tamanho = strlen(comando);

	if (tamanho == 0)
		return true;
	if (!ehLetra(comando[0]) && comando[0] != '_')
		return true;

	for (i = 1; i < tamanho; i++)
		if (!ehLetra(comando[i]) && !ehNumero(comando[i]) && comando[i] != '_')
			return true;

	return false;
}

bool rotuloInvalido(char* rotulo) {
	return comandoInvalido(rotulo);
}

bool processaRotulo(bool* encontrouRotulo, char** rotulo, char* linha, int* i, int* j, int linhaCount) {
	if (*encontrouRotulo) {
		mensagemErro(APENAS_UM_ROTULO_POR_LINHA, linhaCount, linha);
		return false;
	}

	*rotulo = (char *) malloc(sizeof(char) * (*j - *i + 1));
	*encontrouRotulo = true;
	substring(*rotulo, linha, *i, *j - *i);

	if (rotuloInvalido(*rotulo)) {
		mensagemErro(ROTULO_INVALIDO, linhaCount, linha);
		return false;
	}

	*i = proximoCaracter(linha, *j + 1);
	*j = *i - 1;

	return true;
}

int encontraRotulo(char* nome, Rotulo* rotulos, int rotulosCount) {
	int i;

	if (rotulos == NULL)
		return -1;

	trim(nome);

	for (i = 0; i < rotulosCount; i++)
		if (saoIguais(nome, rotulos[i]->nome))
			return i;

	return -1;
}

Rotulo novoRotulo(char* nome, int enderecoMemoria, bool esquerda) {
	Rotulo rotulo = (Rotulo) malloc(sizeof(_rotulo));

	substring(rotulo->nome, nome, 0, length(nome));
	rotulo->esquerda = esquerda;
	rotulo->enderecoMemoria = enderecoMemoria;

	return rotulo;
}

bool adicionaRotulo(char* nome, int enderecoMemoria, bool esquerda, Rotulo** rotulos, int* rotulosCount) {
	Rotulo* aux = *rotulos;
	if (encontraRotulo(nome, *rotulos, *rotulosCount) != -1)
		return false;

	*rotulosCount = *rotulosCount + 1;

	aux = (Rotulo *) realloc(aux, sizeof(Rotulo) * (*rotulosCount  + 1));
	aux[*rotulosCount - 1] = novoRotulo(nome, enderecoMemoria, esquerda);

	*rotulos = aux;

	return true;
}

bool substituiRotuloPorEndereco(Comando *comando, Rotulo* rotulos, int rotulosCount) {
	Comando aux = *comando;
	int i;

	if (aux == NULL || ehVazia(aux->parametroStr))
		return true;

	for (i = 0; i < rotulosCount; i++) {
		if (saoIguais(aux->parametroStr, rotulos[i]->nome)) {
			aux->parametro = rotulos[i]->enderecoMemoria;
			aux->instrucao = alteraInstrucaoDeAcordoComRotulo(aux->instrucao, rotulos[i]->esquerda);
			*comando = aux;
			return true;
		}
	}

	return false;
}
