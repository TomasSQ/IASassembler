#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "stringUtils.h"
#include "diretiva.h"
#include "instrucao.h"
#include "rotulo.h"

#define PARAMETRO_ARQUIVO_ENTRADA 1
#define PARAMETRO_ARQUIVO_SAIDA 2

bool caracterValido(char c) {
	return ehLetra(c) || ehCaracterEspecial(c) || ehEspaco(c) || ehNumero(c) || c == ':';
}

bool primeiroCaracterValido(char c) {
	return caracterValido(c) || !ehNumero(c);
}

void excluiComentario(char* linha) {
	excluirAteCaracter(linha, '#');
}

bool escreveNoArquivoDeSaida(char* nomeArquivoSaida, Linha* linhas, int linhasCount, Rotulo* rotulos, int rotulosCount) {
	int i;
	bool faltouRotulo = false;
	FILE* arquivoSaida = fopen(nomeArquivoSaida, "w");

	if (arquivoSaida == NULL)
		return false;

	ordenarPorEnderecoDeMemoria(&linhas, linhasCount);

	for (i = 0; i < linhasCount; i++) {
		if (linhas[i]->ehDado)
			fprintf(arquivoSaida, "%03X %s\n", linhas[i]->enderecoMemoria, numeroFormatado(linhas[i]->dado));
		else {
			if (!substituiRotuloPorEndereco(&(linhas[i]->esquerda), rotulos, rotulosCount)) {
				printf("Rotulo %s nao declarado\n", linhas[i]->esquerda->parametroStr);
				faltouRotulo = true;
				break;
			}

			fprintf(arquivoSaida, "%03X %02X %03X", linhas[i]->enderecoMemoria, linhas[i]->esquerda->instrucao, linhas[i]->esquerda->parametro);

			if (linhas[i]->direita == NULL)
				fprintf(arquivoSaida, " 00 000\n");
			else {
				if (!substituiRotuloPorEndereco(&(linhas[i]->direita), rotulos, rotulosCount)) {
					printf("Rotulo \"%s\" nao declarado\n", linhas[i]->direita->parametroStr);
					faltouRotulo = true;
					break;
				}

				fprintf(arquivoSaida, " %02X %03X\n", linhas[i]->direita->instrucao, linhas[i]->direita->parametro);
			}
		}
	}

	fclose(arquivoSaida);

	if (faltouRotulo)
		remove(nomeArquivoSaida);

	return !faltouRotulo;
}

bool escreveMapa(char* rotulo, char* diretiva, char* instrucao, char parametro[2][STREAM_SIZE], Linha** linhas, int* linhasCount, int *enderecoMemoria, bool* esquerda, char* linha, int linhaCount) {
	if (ehVazia(diretiva) && ehVazia(instrucao)) /* linha com apenas rotulo, comentario ou vazia*/
		return true;

	if (!escreveDiretiva(linhas, linhasCount, diretiva, parametro, enderecoMemoria, esquerda, linha, linhaCount))
		return false;

	escreveInstrucao(linhas, linhasCount, instrucao, parametro, enderecoMemoria, esquerda);

	return true;
}

bool processaLinha(char* linha, char** rotulo, char* diretiva, char sets[MAX_SETS][STREAM_SIZE], char* instrucao, char parametro[2][STREAM_SIZE], int linhaCount) {
	int inicioPalavra, fimPalavra, tamanho;
	bool encontrouRotulo, encontrouDiretiva, encontrouInstrucao;
	encontrouRotulo = false;
	encontrouDiretiva = false;
	encontrouInstrucao = false;
	tamanho = strlen(linha); 

	if (*rotulo != NULL)
		*rotulo = NULL;

	zerarString(diretiva);
	zerarString(instrucao);
	zerarString(parametro[0]);
	zerarString(parametro[1]);

	inicioPalavra = 0;
	for (fimPalavra = 0; fimPalavra < tamanho; fimPalavra++) {
		if (!caracterValido(linha[fimPalavra])) {
			mensagemErro(CARACTER_INVALIDO, linhaCount, linha);
			return false;
		}

		switch (linha[fimPalavra]) {
			case ':' : {	/* encontrado fim de rotulo */
				if (!processaRotulo(&encontrouRotulo, rotulo, linha, &inicioPalavra, &fimPalavra, linhaCount))
					return false;
				break;
			}
			case '.' : {	/* encontrado comeco de diretiva */
				inicioPalavra++; /* pula-se o '.' */
				fimPalavra++;
				if (!processaDiretiva(&encontrouDiretiva, diretiva, sets, parametro, linha, &inicioPalavra, &fimPalavra, linhaCount))
					return false;
				break;
			}
			case '\r' :
			case '\0':		/* encontrado fim de instrucao sem parametro */
			case '\t' :
			case ' ' : {	/* encontrado fim do nome da instrucao, parametros viram depois */
				if (!processaInstrucao(&encontrouInstrucao, instrucao, sets, parametro, linha, &inicioPalavra, &fimPalavra, linhaCount))
					return false;
				break;
			}
		}

		if (!ehVazia(*rotulo) && instrucaoValida(*rotulo) != -1) {
			mensagemErro(ROTULO_COM_NOME_DE_INSTRUCAO, linhaCount, linha);
			return false;
		}

		if (encontrouDiretiva && encontrouInstrucao) {
			mensagemErro(DIRETIVA_E_INSTRUCAO_NA_MESMA_LINHA, linhaCount, linha);
			return false;
		}
	}

	inicioPalavra = 0;
	fimPalavra = tamanho;
	if (ehVazia(*rotulo) && ehVazia(diretiva) && ehVazia(instrucao)) /* pode ser alguma instrucao sem parametro */
		if (!processaInstrucao(&encontrouInstrucao, instrucao, sets, parametro, linha, &inicioPalavra, &fimPalavra, linhaCount))
			return false;

	return true;
}

bool processa(char* nomeArquivoEntrada, char* nomeArquivoSaida) {
	FILE* arquivoEntrada;
	Linha* linhas = NULL;
	int linhasCount;
	char linha[STREAM_SIZE];
	int linhaCount;
	int enderecoMemoria = 0;
	bool esquerda = true;
	bool sucesso = true;

	char *rotulo = NULL;
	Rotulo* rotulos = NULL;
	int rotulosCount = 0;

	char diretiva[STREAM_SIZE];
	char sets[MAX_SETS][STREAM_SIZE];

	char instrucao[STREAM_SIZE];
	char parametro[2][STREAM_SIZE];

	arquivoEntrada = fopen(nomeArquivoEntrada, "rt");
	if (arquivoEntrada == NULL)
		return false;

	linhaCount = 0;
	while (sucesso && fgets(linha, STREAM_SIZE, arquivoEntrada) != NULL) {
		linhaCount++;

		if (linha[0] == '\n')
			continue;

		/* removendo os comentarios e espacos iniciais e finais fara com que
		*  caso se encontre ':' antes de ' ', pode ser um rotulo
		*  caso se encontre '.', pode ser uma diretiva
		*  caso se encontre ' ', pode ser uma instrucao
		*/
		excluiComentario(linha);
		trim(linha);

		/* pode ser que a linha era apenas de espacos ou comentarios */
		if (ehVazia(linha))
			continue;

		if (!primeiroCaracterValido(linha[0])) {
			mensagemErro(CARACTER_INVALIDO, linhaCount, linha);
			sucesso = false;
			break;
		}

		if (!processaLinha(linha, &rotulo, diretiva, sets, instrucao, parametro, linhaCount)) {
			sucesso = false;
			break;			
		}

		if (!ehVazia(rotulo))
			if (!adicionaRotulo(rotulo, enderecoMemoria, esquerda, &rotulos, &rotulosCount)) {
				mensagemErro(ROTULO_REPETIDO, linhaCount, linha);
				return false;
			}

		if (!escreveMapa(rotulo, diretiva, instrucao, parametro, &linhas, &linhasCount, &enderecoMemoria, &esquerda, linha, linhaCount)) {
			sucesso = false;
			break;
		}

		if (enderecoMemoria > MAX_FILE_SIZE) {
			printf(MAXIMO_DE_INSTRUCOES_ALCANCADO);
			sucesso = false;
			break;
		}
	}

	fclose(arquivoEntrada);

	if (sucesso)
		return escreveNoArquivoDeSaida(nomeArquivoSaida, linhas, linhasCount, rotulos, rotulosCount);

	return sucesso;
}

char* getNomeArquivoSaida(int argc, char** argv) {
	char* nomeArquivoSaida;
	if (argc == 3)
		return argv[PARAMETRO_ARQUIVO_SAIDA];

	nomeArquivoSaida = (char *) malloc(sizeof(char) * (length(argv[PARAMETRO_ARQUIVO_ENTRADA]) + 5));
	sprintf(nomeArquivoSaida, "%s.hex", argv[PARAMETRO_ARQUIVO_ENTRADA]);
	nomeArquivoSaida[length(argv[PARAMETRO_ARQUIVO_ENTRADA]) + 5] = '\0';

	return nomeArquivoSaida;
}

int main(int argc, char** argv) {
	char* nomeArquivoEntrada;
	char* nomeArquivoSaida;

	if (argc < 2 || argc > 3) {
		printf(MODO_DE_USO);
		return 1;
	}

	nomeArquivoEntrada = argv[PARAMETRO_ARQUIVO_ENTRADA];
	nomeArquivoSaida = getNomeArquivoSaida(argc, argv);

	printf(PROCESSANDO_ARQUIVO, nomeArquivoEntrada);

	if (processa(nomeArquivoEntrada, nomeArquivoSaida))
		printf(MAPA_SALVO, nomeArquivoSaida);


	return 0;
}
