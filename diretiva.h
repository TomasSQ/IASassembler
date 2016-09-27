#include "linha.h"

#ifndef DIRETIVA_H
#define DIRETIVA_H

#define DIRETIVA_MAX_LENGTH 5
#define DIRETIVA_MIN_LENGTH 3
#define DIRETIVAS_LENGTH 5

/* 
	retorna a posicao i do vetor de diretivas em que a instrucao se encontra
	align, org, set, wfill", word
	-1 a instrucao seja invalida
*/
int diretivaValida(char* diretiva);

/*
	retorna verdadeiro caso nao tenha diretiva na linha, ou se existe uma diretiva na linha entre j e o final da linha,
	sendo ela valida. encontrouDiretiva tem seu valor alterado para verdadeiro caso seja encontrada a diretiva valida
	em diretiva se encontrara a diretiva, em parametro, os possiveis parametros da diretiva
*/
bool processaDiretiva(bool* encontrouDiretiva, char* diretiva, char sets[MAX_SETS][STREAM_SIZE], char parametro[2][STREAM_SIZE], char* linha, int* i, int* j, int linhaCount);

/* escreve o codigo hex referente a diretiva e seus parametros, retorna falso caso alguma inconsistencia exista */
bool escreveDiretiva(Linha** linhas, int *linhasCount, char* diretiva, char parametro[2][STREAM_SIZE], int* enderecoMemoria, bool* esquerda, char* linha, int linhaCount);

#endif
