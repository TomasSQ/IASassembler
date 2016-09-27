#ifndef LINHA_H
#define LINHA_H

typedef struct comando {
	int instrucao;
	char* parametroStr;
	int parametro;
} _comando ;

typedef _comando* Comando;

typedef struct linha {
	int enderecoMemoria;
	Comando esquerda;
	Comando direita;
	int dado;
	bool ehDado;
} _linha ;

typedef _linha* Linha;

Linha novaLinha(int enderecoMemoria);

int encontraLinha(int enderecoMemoria, Linha* rotulos, int linhasCount);

bool adicionaComando(int enderecoMemoria,  int dado, int instrucao, char* parametro, Linha** linhas, int* linhasCount, bool esquerda);

void ordenarPorEnderecoDeMemoria(Linha** linhas, int linhasCount);

#endif
