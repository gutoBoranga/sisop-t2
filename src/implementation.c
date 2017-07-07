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
                strcat(pathPaiAtual, name_token);
                strcat(pathPaiAtual, "/");
                paiAtual = dirAtual;
              }
        } while(NextFila2(&dirList) == 0 && !tokenEqualsAtual);
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
        if(tokenEqualsAtual && name_token == NULL)
            found = 1;
        if(!tokenEqualsAtual){
            error = 1;
            paiAtual = NULL;
        }
    }
    return paiAtual;
}

void readEntradas(char* sector_buffer) {
    struct t2fs_record registroAux;
    //char *sector_buffer;

    //read_sector(setor, sector_buffer);

    registroAux.TypeVal = *((BYTE *)(sector_buffer));
    strncpy(registroAux.name, sector_buffer + 1, 51);
    registroAux.blocksFileSize = *((DWORD *)(sector_buffer + 52));
    registroAux.bytesFileSize = *((DWORD *)(sector_buffer + 56));
    registroAux.MFTNumber = *((DWORD *)(sector_buffer + 60));


    printf("\n TypeVal = %d", registroAux.TypeVal);
    printf("\n name = %s", registroAux.name);
    printf("\n blocksFileSize = %d", registroAux.blocksFileSize);
    printf("\n bytesFileSize = %d", registroAux.bytesFileSize);
    printf("\n MFTNumber = %d", registroAux.MFTNumber);
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