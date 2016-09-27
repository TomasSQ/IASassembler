#include <string.h>

#include "stringUtils.h"
#include "diretiva.h"

#define ALIGN 0
#define ORG 1
#define SET 2
#define WFILL 3
#define WORD 4

char diretivas[DIRETIVAS_LENGTH][DIRETIVA_MAX_LENGTH + 1] = {"align", "org", "set", "wfill", "word"};

int diretivaValida(char* diretiva) {
	int i;
	int tamanho = length(diretiva);
	
	if (DIRETIVA_MIN_LENGTH > tamanho || tamanho > DIRETIVA_MAX_LENGTH)
		return -1;

	for (i = 0; i < DIRETIVAS_LENGTH; i++)
		if (indiceDaString(diretiva, diretivas[i]) != -1)
			return i;

	return -1;
}

void zerarParametros(char* diretiva, char parametro[2][STREAM_SIZE]) {
	zerarString(diretiva);
	zerarString(parametro[0]);
	zerarString(parametro[1]);
}

void adicionaSet(char* nome, char* valor, char sets[MAX_SETS][STREAM_SIZE]) {
	int i = 0;

	for (i = 0; i < MAX_SETS; i++) {
		if (indiceDaString(sets[i], nome) == 0)
			zerarString(sets[i]);
		if (ehVazia(sets[i])) {
			substring(sets[i], nome, 0, length(nome));
			strcat(sets[i], "|");
			strcat(sets[i], valor);

			break;
		}
	}
}

bool processaDiretiva(bool* encontrouDiretiva, char* diretiva, char sets[MAX_SETS][STREAM_SIZE], char parametro[2][STREAM_SIZE], char* linha, int* i, int* j, int linhaCount) {
	int indiceDiretiva = -1;

	if (*encontrouDiretiva) {
		mensagemErro(MAIS_DE_UMA_DIRETIVA_NA_LINHA, linhaCount, linha);
		zerarParametros(diretiva, parametro);
		return false;
	}

	*encontrouDiretiva = true;

	while (*j < length(linha)) {
		if (linha[*j] == '.') {
			mensagemErro(MAIS_DE_UMA_DIRETIVA_NA_LINHA, linhaCount, linha);
			zerarParametros(diretiva, parametro);
			return false;
		}

		if (linha[*j] == ':') {
			mensagemErro(ROTULO_DEPOIS_DE_DIRETIVA, linhaCount, linha);
			zerarParametros(diretiva, parametro);
			return false;
		}
			
		/* se vai acabar a linha, ou se acabou uma palavra, ou se encontrou fim do primeiro parametro (havendo dois) */
		if (linha[*j + 1] == '\0' || ehEspaco(linha[*j]) || (!ehVazia(diretiva) && ehVazia(parametro[0]) && linha[*j] == ',')) {
			if (linha[*j] == ',' && (indiceDiretiva != WFILL && indiceDiretiva != SET)) {
				mensagemErro(DIRETIVA_INVALIDA, linhaCount, linha);
				zerarParametros(diretiva, parametro);
				return false;
			}
			if (ehVazia(diretiva)) {
				substring(diretiva, linha, *i, *j - *i);

				indiceDiretiva = diretivaValida(diretiva);
				if (indiceDiretiva == -1) {
					mensagemErro(DIRETIVA_INVALIDA, linhaCount, linha);
					zerarParametros(diretiva, parametro);
					return false;
				}
			} else if (ehVazia(parametro[0]) || ehVazia(parametro[1])) {
				if (indiceDiretiva != WFILL && indiceDiretiva != SET && !ehVazia(parametro[0])) { /* Apenas wfill e set aceitam dois parametros */
					mensagemErro(DIRETIVA_COM_MUITOS_PARAMETROS, linhaCount, linha);
					zerarParametros(diretiva, parametro);
					return false;
				}
				
				if (ehVazia(parametro[0]))
					substring(parametro[0], linha, *i, *j - *i + (linha[*j + 1] == '\0'));
				else if (ehVazia(parametro[1])) {
					substring(parametro[1], linha, *i, *j - *i + (linha[*j + 1] == '\0'));
					if (indiceDiretiva == SET)
						adicionaSet(parametro[0], parametro[1], sets);
				} else { /* encontrou um "terceiro" parametro */
					mensagemErro(DIRETIVA_COM_MUITOS_PARAMETROS, linhaCount, linha);
					zerarParametros(diretiva, parametro);
					return false;
				}
			}

			if (linha[*j + 1] != '\0') {
				*i = proximoCaracter(linha, *j + (linha[*j] == ','));
				*j = *i - 1;
			} else 
				*j = length(linha);
		}
		*j = *j + 1;
	}

	return true;
}

bool escreveDiretiva(Linha** linhas, int *linhasCount, char* diretiva, char parametro[2][STREAM_SIZE], int* enderecoMemoria, bool* esquerda, char* linha,  int linhaCount) {
	int i;
	int multiplo;
	int indiceDiretiva;
	int quantasLinhas;

	if (ehVazia(diretiva))
		return true;

	indiceDiretiva = diretivaValida(diretiva);

	switch (indiceDiretiva) {
		case ORG : {
			*enderecoMemoria = (int) strtol(parametro[0], NULL, 0);
			*esquerda = true;
			break;
		}
		case ALIGN : {
			multiplo = (int) strtol(parametro[0], NULL, 0);
			/*
				ex:
				enderecoMemoria = 13, encontrou .align 7
				14 = 13 + 7 - 13 % 7
			*/
			i = multiplo == 1 ? *enderecoMemoria : *enderecoMemoria + multiplo - *enderecoMemoria % multiplo;
			if (!*esquerda) {	/* se estivermos ha direita, "completamos" com 0 */
				*esquerda = true;
				*enderecoMemoria = *enderecoMemoria  + 1;
			}
			/* completa com 0 ate chegar no proximo endereco */
			while (*enderecoMemoria < i) {
				if (!adicionaComando(*enderecoMemoria, 0, -1, NULL, linhas, linhasCount, *esquerda))
					printf(SOBRE_ESCRITA_DE_ENDERECO, *enderecoMemoria);

				*enderecoMemoria = *enderecoMemoria  + 1;
			}
			break;
		}
		case WORD :
		case WFILL : {
			/* word e wfill ocupam os 40 bits, entao devemos estar na esquerda */
			if (!*esquerda) {
				mensagemErro(MAL_ALINHADO, linhaCount, linha);
				return false;
			}

			if (indiceDiretiva == WORD) {
				if (!adicionaComando(*enderecoMemoria, strToInt(parametro[0]), -1, NULL, linhas, linhasCount, *esquerda))
					printf(SOBRE_ESCRITA_DE_ENDERECO, *enderecoMemoria);
				*enderecoMemoria = *enderecoMemoria + 1;
			} else { /* so pode ser wfill */
				quantasLinhas = strToInt(parametro[0]);
				for (i = 0; i < quantasLinhas; i++) {
					if (!adicionaComando(*enderecoMemoria, strToInt(parametro[1]), -1, NULL, linhas, linhasCount, *esquerda))
						printf(SOBRE_ESCRITA_DE_ENDERECO, *enderecoMemoria);
					*enderecoMemoria = *enderecoMemoria + 1;
				}
			}
			break;
		}
	}

	return true;
}

