#include <string.h>

#include "stringUtils.h"
#include "instrucao.h"

/* INSTRUCAO_MAX_LENGTH + 1 para se adicionar o '\0'*/
char instrucoes[INSTRUCOES_LENGTH][INSTRUCAO_MAX_LENGTH + 1] = 
	{"ADD", "ADDABS", "DIV", "JGEZ", "JMP", "LDMQ", "LDMQM", "LDN", "LDABS", "LOAD", "LSH", "MUL", "RSH", "STR", "SUB", "SUBABS", "STM"};
int instrucoesParamCount[INSTRUCOES_LENGTH] =
	{	 1,		   1,	  1,	  1,	 1,		 0,		  1,	 1,		  1,	  1,	 0,		1,	   0,	  1,	 1,		   1,	  1};
int instrucoesCodigo[INSTRUCOES_LENGTH] =
	{	 5,		   7,	 12,	 15,	13,		10,		  9,	 2,		  3,	  1,	20,	   11,	   21,	 33,	 6,		   8,	 18};

int instrucaoValida(char* instrucao) {
	int i;
	int tamanho = length(instrucao);
	
	if (INSTRUCAO_MIN_LENGTH > tamanho || tamanho > INSTRUCAO_MAX_LENGTH)
		return -1;

	for (i = 0; i < INSTRUCOES_LENGTH; i++)
		if (indiceDaString(instrucao, instrucoes[i]) != -1 && length(instrucao) == length(instrucoes[i]))
			return i;

	return -1;
}

void zerarInstrucao(char* instrucao, char parametro[2][STREAM_SIZE]) {
	zerarString(instrucao);
	zerarString(parametro[0]);
}

bool parametroValido(char* parametro, char sets[MAX_SETS][STREAM_SIZE]) {
	int i;
	int indiceAbreParenteses = indiceDoCaracter(parametro, '(');
	int indiceFechaParenteses = indiceDoCaracter(parametro, ')');

	if (ehVazia(parametro))
		return false;

	
	/* se comeca por numero */
	if (ehNumero(parametro[0]))
		return false;

	/* se tem parenteses */
	if (indiceAbreParenteses != -1 || indiceAbreParenteses != -1) {
		/* se o parenteses esta na posicao errada */
		if (indiceAbreParenteses != 1 || indiceFechaParenteses != length(parametro) - 1)
			return false;

		/* se nao tem M ou m */
		if (indiceDoCaracter(parametro, 'm') != 0)
			return false;

		/* se o conteudo entre os parenteses nao for um numero (ou "set") */
		substring(parametro, parametro, indiceAbreParenteses + 1, indiceFechaParenteses - indiceAbreParenteses - 1);
		if (!ehNumerica(parametro)) {
			for (i = 0; i < MAX_SETS; i++)
				if (indiceDaString(sets[i], parametro) != -1) {
					substring(parametro, sets[i], indiceDoCaracter(sets[i], '|') + 1, 20);
					return true;
				}
			return false;
		}
	} else if (ehNumerica(parametro)) /* faltou M(..), pois o parametro nao era um rotulo, nem set */
		return false;

	return true;
}

bool processaInstrucao(bool* encontrouInstrucao, char* instrucao, char sets[MAX_SETS][STREAM_SIZE], char parametro[2][STREAM_SIZE], char* linha, int* i, int* j, int linhaCount) {
	int qualInstrucao;
	if (*encontrouInstrucao) {
		mensagemErro(MAIS_DE_UMA_INSTRUCAO_NA_LINHA, linhaCount, linha);
		zerarInstrucao(instrucao, parametro);
		return false;
	}

	*encontrouInstrucao = true;
	substring(instrucao, linha, *i, *j - *i);
	qualInstrucao = instrucaoValida(instrucao);

	if (qualInstrucao == -1) {
		mensagemErro(INSTRUCAO_INVALIDA, linhaCount, linha);
		return false;
	}

	if (instrucoesParamCount[qualInstrucao] < 1 && linha[*j] != '\0') {
		mensagemErro(INSTRUCAO_COM_MUITOS_PARAMETROS, linhaCount, linha);
		zerarInstrucao(instrucao, parametro);
		return false;
	}

	*i = proximoCaracter(linha, *j + 1);
	*j = *i;

	while (*j < length(linha)) {
		if (linha[*j] == ':') {
			mensagemErro(ROTULO_DEPOIS_DE_INSTRUCAO, linhaCount, linha);
			zerarInstrucao(instrucao, parametro);
			return false;
		}
		if (linha[*j] == '.') {
			mensagemErro(DIRETIVA_DEPOIS_DE_INSTRUCAO, linhaCount, linha);
			zerarInstrucao(instrucao, parametro);
			return false;
		}

		if (!ehLetra(linha[*j]) && !ehNumero(linha[*j]) && linha[*j] != '_' && linha[*j] != '(' && linha[*j] != ')') {
			mensagemErro(PARAMETRO_INVALIDO, linhaCount, linha);
			zerarInstrucao(instrucao, parametro);
			return false;
		}

		/* se vai acabar a linha, ou se acabou o parametro */
		if (linha[*j + 1] == '\0' || ehEspaco(linha[*j])) {
			if (ehVazia(parametro[0])) {
				substring(parametro[0], linha, *i, *j - *i + (linha[*j + 1] == '\0'));
				if (!parametroValido(parametro[0], sets)) {
					mensagemErro(PARAMETRO_INVALIDO, linhaCount, linha);
					zerarInstrucao(instrucao, parametro);
					return false;
				}
			} else { /* Todas as instrucoes tem no maximo um parametro */
				mensagemErro(INSTRUCAO_COM_MUITOS_PARAMETROS, linhaCount, linha);
				zerarInstrucao(instrucao, parametro);
				return false;
			}

			/* continua leitura da linha pois podem haver erros de sintaxe */
			if (linha[*j + 1] != '\0') {
				*i = proximoCaracter(linha, *j + (linha[*j] == ','));
				*j = *i - 1;
			} else
				*j = length(linha);
		}
		*j = *j + 1;
	}

	if (instrucoesParamCount[qualInstrucao] > 0 && ehVazia(parametro[0])) {
		mensagemErro(INSTRUCAO_COM_POUCOS_PARAMETROS, linhaCount, linha);
		zerarInstrucao(instrucao, parametro);
		return false;
	}

	return true;
}

bool escreveInstrucao(Linha** linhas, int *linhasCount, char* instrucao, char parametro[2][STREAM_SIZE], int* enderecoMemoria, bool* esquerda) {
	int indiceInstrucao;

	if (ehVazia(instrucao))
		return true;

	indiceInstrucao = instrucaoValida(instrucao);
	if (indiceInstrucao == -1)
		return false;

	if (!adicionaComando(*enderecoMemoria, -1, instrucoesCodigo[indiceInstrucao], parametro[0], linhas, linhasCount, *esquerda))
		printf(SOBRE_ESCRITA_DE_ENDERECO, *enderecoMemoria);

	*esquerda = !*esquerda;

	if (*esquerda)
		*enderecoMemoria = *enderecoMemoria + 1;

	return true;
}

int alteraInstrucaoDeAcordoComRotulo(int codigo, bool esquerda) {
	switch (codigo) {
		case 13:
		case 15:
		case 18:
			return codigo + !esquerda;
		default :
		return codigo;
	}
}

