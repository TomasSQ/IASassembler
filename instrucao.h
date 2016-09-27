#include "linha.h"

#ifndef INSTRUCAO_H
#define INSTRUCAO_H

#define INSTRUCAO_MAX_LENGTH 6
#define INSTRUCAO_MIN_LENGTH 3
#define INSTRUCOES_LENGTH 17

/* 
	retorna a posicao i do vetor de instrucoes em que a instrucao se encontra
		ADD, ADDABS, DIV, JGEZ, JMP, LDABS, LDMQ, LDMQM, LDN, LOAD, LSH, MUL, RSH, STM, STR, SUB, SUBABS
	-1 a instrucao seja invalida
*/
int instrucaoValida(char* instrucao);

/* retorna quantos parametros a instrucao aceita, -1 caso seja uma instrucao invalida */
int quantosParametros(char* instrucao);

/*
	retorna verdadeiro caso nao tenha instrucao na linha, ou se existe uma instrucao na linha entre j e o final da linha,
	sendo ela valida. encontrouInstrucao tem seu valor alterado para verdadeiro caso seja encontrada a instrucao valida
	em instrucao se encontrara a instrucao, em parametro, os possiveis parametros da instrucao
*/
bool processaInstrucao(bool* encontrouInstrucao, char* instrucao, char sets[MAX_SETS][STREAM_SIZE], char parametro[2][STREAM_SIZE], char* linha, int* i, int* j, int linhaCount);

/* escreve o codigo hex referente a diretiva e seus parametros, retorna falso caso alguma inconsistencia exista */
bool escreveInstrucao(Linha** linhas, int* linhasCount, char* instrucao, char parametro[2][STREAM_SIZE], int* enderecoMemoria, bool* esquerda);

/*
 retorna o codigo da instrucao, de acordo a posicao do rotulo (esquerda)
 se o codigo original for JMP, JGEZ, STM e esquerda for falso, incrementa um no codigo da instrucao
 */
int alteraInstrucaoDeAcordoComRotulo(int codigo, bool esquerda);

#endif
