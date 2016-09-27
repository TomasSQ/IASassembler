#include <stdlib.h>
#include <stdio.h>

#define MAX_ROTULOS 2048 /* um rotulo/diretiva/instrucao por endereco de memoria, vezes dois (esquerda direita) */
#define MAX_SETS 2048
/* o arquivo pode ter no maximo 1024 linhas, cada linha com duas instrucoes ou um dado */
#define MAX_FILE_SIZE 1024

enum boolean {false, true};
typedef enum boolean bool;
