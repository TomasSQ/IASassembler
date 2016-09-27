#include "common.h"
#include "mensagens.h"

#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#define STREAM_SIZE 2048

/* faz com que string[i] = '\0', para qualquer i menor do que length(string) */
void zerarString(char *string);

/* retorna se a string comeca com '\0' */
bool ehVazia(char* string);

/* retorna o tamanho de uma string */
int length(char* string);

/* Coloca em sub string[i..j] */
void substring(char* sub, char* string, int i, int j);

void replace(char** s, char* palavraAntiga, char* palavraNova, int posicaoPalavraAntiga);

/* retorna verdadeiro se c for uma letra sem ascento (A-Zaz) */
bool ehLetra(char c);

/* retorna verdadeiro se c for um numero */
bool ehNumero(char c);

/* retorna verdadeiro se a string s for um numero */
bool ehNumerica(char* s);

/* retorna verdadeiro se c ponto, underscore, ou serquilha */
bool ehCaracterEspecial(char c);

/* retorna verdadeiro se c for espaco, tabulacao ou quebra de linha */
bool ehEspaco(char c);

/*
Mantem a string com mesmo tamanho em memoria, mas
adiciona o caracter de fim de string na posicao do
delimitador
*/
void excluirAteCaracter(char *string, char delimitador);

/* retorna o primeiro indice i de string, tal que i >= comeco E ehEspaco(string[i]) = false */
int proximoCaracter(char *string, int comeco);

/*
Remove todos os espacos, tabulacoes e quebras de linha
da esquerda para a direita ate encontrar um caracter com representacao visual
e da direita para a esquerda ate encontrar um caracter com representacao visual
*/
void trim(char* string);

/* 
	retorna
		o indice em que string[indice] == c, ou
		-1 caso c nao exista em string, ou
		-2 caso exista mais de uma vez
*/
int indiceDoCaracter(char *string, char c);

/* 
	retorna o indice em que comeca a string s em string
		-1 caso tal indice nao exista
*/
int indiceDaString(char *string, char* s);

/* 
	retorna verdadeiro se length(string) == length(s) e indiceDaString(string, s) == 0
*/
bool saoIguais(char *string, char* s);

/* retorna verdadeiro se c for uma letra, espaco, tabulacao, quebra de linha, ou caracter especial */
bool ehCaracterValido(char c);

/* Escreve na saida padrao:
Na linha: qualLinha
linha
erro
*/
void mensagemErro(char* erro, int qualLinha, char* linha);

/*
	retorna a respresentacao do numero no formato DD DDD DD DDD
*/
char* formatarNumero(char* numero);
char* numeroFormatado(int n);


/* retorna uma string com a representacao hex do inteiro i*/
char* intToStrHex(int i);

int strToInt(char* s);

#endif
