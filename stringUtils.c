#include <string.h>

#include "stringUtils.h"

void zerarString(char* string) {
	int i;

	for (i = 0; i < length(string); i++)
		string[i] = '\0';
}

bool ehVazia(char* string) {
	return string == NULL || string[0] == '\0';
}

int length(char* string) {
	if (string == NULL)
		return 0;

	return (int) strlen(string);
}

bool ehNumero(char c) {
	return c >= '0' && c <= '9';
}

bool ehNumerica(char* s) {
	char* aux;
	if (ehVazia(s))
		return false;
	strtol(s, &aux, 0);

	return aux == NULL || aux[0] == '\0';
}

bool ehLetra(char c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool ehCaracterEspecial(char c) {
	return c == '#' || c == '_' || c == '.';
}
bool ehEspaco(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\0' || c == '\r';
}

void substring(char* sub, char* string, int i, int n) {
	strncpy(sub, string + i * sizeof(char), n);
	sub[n] = '\0';
}

void replace(char** s, char* palavraAntiga, char* palavraNova, int posicaoPalavraAntiga) {
	int novoTamanho = length(*s) - length(palavraAntiga) + length(palavraNova);

	char* aux = (char *) malloc(sizeof(char) * novoTamanho);
	char* aux2 = (char *) malloc(sizeof(char) * novoTamanho);

	substring(aux, *s, 0, posicaoPalavraAntiga);
	substring(aux2, *s, posicaoPalavraAntiga + length(palavraAntiga), length(*s) - posicaoPalavraAntiga - length(palavraAntiga));
	strcat(aux, palavraNova);
	strcat(aux, aux2);

	strcpy(*s, aux);
}

void excluirAteCaracter(char* string, char delimitador) {
	char* final = strchr(string, delimitador);

	if (final != NULL)
		final[0] = '\0';
}

int proximoCaracter(char *string, int comeco) {
	int i = comeco;
	while (string[i] != '\0' && ehEspaco(string[i]) && i < length(string)) /* encontra primeiro caracter visual */
		i++;

	return i;
}

void trim(char* string) {
	int i, j, k, tamanho;
	char stringTrim[STREAM_SIZE];

	tamanho = length(string);

	i = proximoCaracter(string, 0);
	j = tamanho - 1;
	while (ehEspaco(string[j]) && j > i) /* encontra ultimo caracter visual */
		j--;

	k = 0;
	while (i <= j)
		stringTrim[k++] = string[i++];

	stringTrim[k] = '\0';

	strcpy(string, stringTrim);
}

bool mesmoCaracter(char c, char d) {
	if (c == d)
		return true;

	/* se c e d forem letras, compara "ignorecase" */
	if (ehLetra(c) && ehLetra(d)) {
		if (c < 'a') /* c eh maiusculo, e d minusculo */
			return d - ('a' - 'A') == c;
		/* d eh maiusculo, e c minusculo */
		return c - ('a' - 'A') == d;
	}

	return false;
}

int indiceDoCaracter(char *string, char c) {
	int i;
	int indice = -1;

	for (i = 0; i < length(string); i++)
		if (mesmoCaracter(string[i], c)) {
			if (indice != -1)
				return -2;
			indice = i;
		}

	return indice;
}

int indiceDaString(char *string, char *s) {
	int i, j, indice;
	int tamanhoString = length(string);
	int tamanhoS = length(s);

	if (tamanhoS > tamanhoString)
		return -1;

	indice = -1;
	j = 0;
	for (i = 0; i < tamanhoString; i++) {
		if (s[j] == '\0')
			return indice;
		if (mesmoCaracter(string[i], s[j])) {
			if (indice == -1)
				indice = i;

			j++;
			if (s[j] == '\0')
				return indice;
		} else {
			j = 0;
			indice = -1;
		}
	}

	return -1;
}

bool saoIguais(char* s, char* s2) {
	return length(s) == length(s2) && indiceDaString(s, s2) == 0;
}


void mensagemErro(char* erro, int qualLinha, char* linha) {
	printf("Na linha: %d\n%s\n %s\n", qualLinha, linha, erro);
}

char* formatarNumero(char* numero) {
	char formatado[14]; /* DD DDD DD DDD\0*/

	sprintf(formatado, "%010X", (int) strtol(numero, NULL, 0));
	formatado[13] = '\0';
	formatado[12] = formatado[9];
	formatado[11] = formatado[8];
	formatado[10] = formatado[7];
	formatado[9] = ' ';
	formatado[8] = formatado[6];
	formatado[7] = formatado[5];
	formatado[6] = ' ';
	formatado[5] = formatado[4];
	formatado[4] = formatado[3];
	formatado[3] = formatado[2];
	formatado[2] = ' ';

	strncpy(numero, formatado, 14);

	return numero;
}

char* numeroFormatado(int n) {
	char* formatado = (char*) malloc(sizeof(char) * 14); /* DD DDD DD DDD\0*/

	sprintf(formatado, "%010X", n);
	formatado[13] = '\0';
	formatado[12] = formatado[9];
	formatado[11] = formatado[8];
	formatado[10] = formatado[7];
	formatado[9] = ' ';
	formatado[8] = formatado[6];
	formatado[7] = formatado[5];
	formatado[6] = ' ';
	formatado[5] = formatado[4];
	formatado[4] = formatado[3];
	formatado[3] = formatado[2];
	formatado[2] = ' ';

	return formatado;
}

char* intToStrHex(int i) {
	char* aux = (char *) malloc(sizeof(char) * 5);
	sprintf(aux, "%03X", i);

	return aux;
}


int strToInt(char* s) {
	if (!ehNumerica(s))
		return -1;

	return (int) strtol(s, NULL, 0);
}
