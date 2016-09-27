#include "linha.h"


#ifndef ROTULO_H
#define ROTULO_H

#define ROTULO_MAX_LENGTH 64
#define ROTULO_MIN_LENGTH 1

typedef struct rotulo {
	char nome[ROTULO_MAX_LENGTH];
	int enderecoMemoria;
	bool esquerda;
} _rotulo;

typedef _rotulo* Rotulo;

bool rotuloInvalido(char* rotulo);

bool processaRotulo(bool* encontrouRotulo, char** rotulo, char* linha, int* i, int* j, int linhaCount);

int encontraRotulo(char* nome, Rotulo* rotulos, int rotulosCount);

Rotulo novoRotulo(char* nome, int enderecoMemoria, bool esquerda);

bool adicionaRotulo(char* rotulo, int enderecoMemoria, bool esquerda, Rotulo** rotulos, int* rotulosCount);

bool substituiRotuloPorEndereco(Comando *comando, Rotulo* rotulos, int rotulosCount);

#endif
