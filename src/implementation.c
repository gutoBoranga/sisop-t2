#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <t2fs.h>
#include <support.h>
#include <implementation.h>

// functions

int writeRegisterToDisk(struct t2fs_record record) {
  return 0;
}

/*----------------------------------------------------------------------------------------------
Função que busca o diretório pai de um arquivo/diretório a partir de seu pathname.
A busca é feita tanto na lista de diretórios abertos quanto no disco.

Retorna: DIRETORIO* --Ponteiro para a struct do diretório pai se encontrou
	 NULL --Caso contrário

GABRIEL JOB
----------------------------------------------------------------------------------------------*/
DIRETORIO* buscaDiretorioPai(char *pathname, int pathname_len, FILA2 dirList) {
    DIRETORIO *paiAtual, *dirAtual;
    registro_dir *entradaAtual;
    char pathcpy[pathname_len], pathPaiAtual[pathname_len];
    char* name_token;
    int found = 0, error = 0, tokenEqualsAtual = 0;

    printf("\n\n\n\n");
    printaDiretoriosLista(dirList);
    printf("\n\n\n\n");
    
    strcpy(pathcpy, pathname);
    name_token = strtok(pathcpy,"/");
    strcpy(pathPaiAtual, "/");

    while(!found && !error){
	     tokenEqualsAtual = 0;
       FirstFila2(&dirList);
       do {
         dirAtual = GetAtIteratorFila2(&dirList);
         printf("\ntoken: %s \ndirAtual: %s \npathPaiAtual: %s \npaiDiretorio: %s\n",name_token,dirAtual->name,pathPaiAtual,dirAtual->pai_pathname);
         if(strcmp(name_token, dirAtual->name) == 0)
              if(strcmp(pathPaiAtual, dirAtual->pai_pathname) == 0) {
                tokenEqualsAtual = 1;
		if (strcmp(pathPaiAtual, "/") != 0)
			strcat(pathPaiAtual, "/");
		strcat(pathPaiAtual, name_token);
              }
        } while(NextFila2(&dirList) == 0 && tokenEqualsAtual==0);
        /*if(dirAtual == NULL){
            FirstFila2(paiAtual->entradas);
            entradaAtual = GetAtIteratorFila2(paiAtual->entradas);
            while(entradaAtual != NULL){
                if(strcmp(name_token, entradaAtual.name) == 0){
                    strcat(pathPaiAtual, "/");
                    strcat(pathPaiAtual, name_token);
                    opendir2(pathPaiAtual);  //apenas cria uma estrutura nova se for realmente um diretório
                    tokenEqualsAtual = 1;
                }
                else
                    NextFila2(paiAtual->entradas);
            }
        }*/
        name_token = strtok(NULL, "/");
	      printf("\ntokenEqualsAtual = %d",tokenEqualsAtual);
        if(tokenEqualsAtual){
		if(name_token == NULL) 
			found = 1;
		else 
			paiAtual = dirAtual;
	}
        if(!tokenEqualsAtual){
            error = 1;
            paiAtual = NULL;
        }
    }
    return paiAtual;
}

/*----------------------------------------------------------------------------------------------
Função para ler as entradas de um diretório do disco.


Retorna: 0  --Se entradas lidas com sucesso
	 -1 --Caso houve erro na leitura

GABRIEL JOB
----------------------------------------------------------------------------------------------*/
int readEntradas(int dirByteSize, PFILA2 entradasList) {
	struct t2fs_record *registroAux;
	char *blocoAtual;	
	int i;	

	int blocoAtualNumber = getBlockFromMFT(); //criar getBlockFromMFT(), cabeçalho abaixo VVV
	while(blocoAtualNumber > 0){
		blocoAtual = readBlock(blocoAtualNumber);
		if(blocoAtual == NULL)
			return -1;
		for(i = dirByteSize; i>0; i = i - 64){ //tamanho t2fs_record=512
			registroAux = malloc(sizeof(struct t2fs_record));

			registroAux->TypeVal = *((BYTE *)(blocoAtual));
    			strncpy(registroAux->name, blocoAtual + 1, 51);
    			registroAux->blocksFileSize = *((DWORD *)(blocoAtual + 52));
    			registroAux->bytesFileSize = *((DWORD *)(blocoAtual + 56));
    			registroAux->MFTNumber = *((DWORD *)(blocoAtual + 60));

			AppendFila2(entradasList, registroAux);
		}
		blocoAtualNumber = getBlockFromMFT();
	}
	return 0;
}

/*----------------------------------------------------------------------------------------------
Função que diz onde está no disco o próximo bloco do arquivo a ser acessado.

PS.: Provavelmente é uma boa usar como parâmetro o último bloco já acessado
----------------------------------------------------------------------------------------------*/
int getBlockFromMFT(){
	return 0;
}

/*----------------------------------------------------------------------------------------------
Função para ler um bloco inteiro do disco.

Retorna: char* --Bytes lidos se lido com sucesso
	 NULL  --Caso houve erro na leitura

GABRIEL JOB
----------------------------------------------------------------------------------------------*/
char* readBlock(int sectorNumber){
	int i;
	int bytesLidos = 0;
	char *sectorBuffer;
	char *blockBuffer;

	for(i=0; i<SECTORS_PER_BLOCK; i++){
		if(read_sector(sectorNumber + i, sectorBuffer) != 0)
			return NULL;
		memcpy(blockBuffer + bytesLidos, sectorBuffer, SECTOR_SIZE);
		bytesLidos = bytesLidos + SECTOR_SIZE;
	}
	return blockBuffer;
}


int printaDiretoriosLista(FILA2 fila) {
  int erro = 0;

  if (FirstFila2(&fila) != 0) {
    printf("\n[fila vazia]");
    return -1;
  }

  DIRETORIO *dirAtual;
  dirAtual = GetAtIteratorFila2(&fila);

  printf("\n[");
  while (dirAtual != NULL) {
    printf("%s ", dirAtual->name);
    NextFila2(&fila);
    dirAtual = GetAtIteratorFila2(&fila);
  }
  printf("]\n");
}
