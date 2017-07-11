#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <t2fs.h>
#include <support.h>
#include <implementation.h>
#include <mft_manager.h>

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
DIRETORIO* buscaDiretorioPai(char *pathname, int pathname_len) { //mudar dirList para uma variavel global
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
         dirAtual = (DIRETORIO *) GetAtIteratorFila2(&dirList);
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
Função que busca um diretório a partir de seu handle na lista de diretórios abertos.


Retorna: DIRETORIO* --Ponteiro para a struct do diretório se encontrou
	 NULL 	    --Caso contrário

GABRIEL JOB
----------------------------------------------------------------------------------------------*/
DIRETORIO* getDiretorio(DIR2 handle){ //mudar dirList para uma variavel global
	DIRETORIO *dirAtual;

	if(FirstFila2(&dirList) == 0){
		do{
			dirAtual = (DIRETORIO *) GetAtIteratorFila2(&dirList);
			if(dirAtual->handle == handle)
				return dirAtual;
		}while(NextFila2(&dirList) == 0);
	}
	return NULL;
}

/*----------------------------------------------------------------------------------------------
Função que pega uma entrada de um diretório de acordo com o valor de current_entry.

Retorna: DIRETORIO* --Ponteiro para a struct da entrada se encontrou
	 NULL 	    --Caso contrário

GABRIEL JOB
----------------------------------------------------------------------------------------------*/
registro_dir* getEntrada(DIRETORIO *dir){
	registro_dir *registroAtual;
	int i;

	if(FirstFila2(&dir->entradas) == 0){
		for(i = 0; i < dir->current_entry; i++){
			if(NextFila2(&dir->entradas) != 0)
				return NULL;
		}
		registroAtual = (registro_dir *) GetAtIteratorFila2(&dir->entradas);
		dir->current_entry = dir->current_entry + 1;
		return registroAtual;
	}
	return NULL;
}

/*----------------------------------------------------------------------------------------------
Função que busca o número de um registro de MFT disponível.

Retorna: MFTNumber --Número do registro caso encontrado
		-1 			--Caso não encontrou

GABRIEL JOB
----------------------------------------------------------------------------------------------*/
int busca_regMFT_livre() {
	reg_MFT *regAtual;
	struct 	t2fs_4tupla *tupla;

	FirstFila2(&area_MFT);
	NextFila2(&area_MFT);
	NextFila2(&area_MFT);
	NextFila2(&area_MFT);
  if (NextFila2(&area_MFT) != 0) { //vai para o primeiro registro não reservado as S.A.
    return -2;
  }
  
	int MFTNumber = 5;
	do{
		regAtual = (reg_MFT *) GetAtIteratorFila2(&area_MFT);
		FirstFila2(&regAtual->tuplas);		//pega a primeira tupla do registro
		tupla = (struct t2fs_4tupla *) GetAtIteratorFila2(&regAtual->tuplas);
		if(tupla->atributeType == -1)
			return MFTNumber;
		
		MFTNumber++;
	} while(NextFila2(&area_MFT) == 0);
	
	return -1;
}

/*----------------------------------------------------------------------------------------------
Função que busca o número de um registro de MFT disponível.

Retorna: MFTNumber --Número do registro caso encontrado
		-1 			--Caso não encontrou

GABRIEL JOB
----------------------------------------------------------------------------------------------*/
reg_MFT* busca_regMFT(int MFTNumber) {
	reg_MFT *regAtual;
	
	if(FirstFila2(&area_MFT) != 0)
		return NULL;
	
	int i = 0;
	for(i = 0; i < MFTNumber; i++){
		if(NextFila2(&area_MFT) != 0)
			return NULL;
	}
	
	regAtual = (reg_MFT *) GetAtIteratorFila2(&area_MFT);
	return regAtual;
}

/*----------------------------------------------------------------------------------------------
Função para ler as entradas de um diretório do disco.


Retorna: 0  --Se entradas lidas com sucesso
	 	-1  --Caso houve erro na leitura

GABRIEL JOB
----------------------------------------------------------------------------------------------*/
int readEntradas(int dirByteSize, PFILA2 entradasList ) {//ADICIONAR MFT_RECORD COMO PARAMETRO
	struct t2fs_record *registroAux;
	unsigned char blocoAtual[BLOCK_SIZE];
	int i, offset = 0;

	int blocoAtualNumber = getBlockFromMFT(); //mudar
	while(blocoAtualNumber > 0){
		if(readBlock(blocoAtualNumber, blocoAtual) != 0)
			return -1;

		offset = 0;

		if(dirByteSize >= BLOCK_SIZE)
			i = BLOCK_SIZE;
		else
			i = dirByteSize;
		while(i>0){
			registroAux = malloc(sizeof(struct t2fs_record));

			registroAux->TypeVal = *((BYTE *)(blocoAtual + offset));
   			strncpy(registroAux->name, blocoAtual + offset + 1, 51);
   			registroAux->blocksFileSize = *((DWORD *)(blocoAtual + offset + 52));
   			registroAux->bytesFileSize = *((DWORD *)(blocoAtual + offset + 56));
   			registroAux->MFTNumber = *((DWORD *)(blocoAtual +offset + 60));

			offset += 64;
			dirByteSize -= 64;
			i -= 64;
			AppendFila2(entradasList, registroAux);
		}
		blocoAtualNumber = getBlockFromMFT();
	}
	return 0;
}

int readEntradas2(FILA2 tuplas, PFILA2 entradasList) {
  if (FirstFila2(&tuplas) != 0) {
    printf("[ERRO] Erro ao ler entradas: fila de tuplas vazia\n\n");
    return -1;
  }
  
  // pega tupla inicial do arquivo
  struct t2fs_4tupla *tuplaAtual;
  tuplaAtual = GetAtIteratorFila2(&tuplas);
  
  // percorre todas tuplas
  while (tuplaAtual != NULL) {
    // se não for uma "tupla de mapeamento VBN-LBN"
    if (tuplaAtual->atributeType != 1) {
      // se for um registro livre
      if (tuplaAtual->atributeType == -1) {
        return -1;
      }
      // se não, ou é registro MFT adicional ou é marcador de fim de encadeamento
      // neste caso, já deve ter colocado na lista todos os registros, então retorna 0
      return 0;
    }
    
    int i, j, k;
    i = j = k = 0;
    
    // percorre todos blocos contíguos da tupla
    for(i = 0; i < tuplaAtual->numberOfContiguosBlocks; i++) {
      int block = tuplaAtual->logicalBlockNumber + i;
      
      // percorre todos setores do bloco
      for(j = 0; j < SECTORS_PER_BLOCK; j++) {
        int sector = block * SECTORS_PER_BLOCK + j;
        
        // percorre todos registros do setor
        for(k = 0; k < RECORDS_PER_SECTOR; k++) {
          struct t2fs_record *record = malloc(sizeof(struct t2fs_record));
          
          if (readRecord(sector, k, record) != 0) {
            printf("[ERRO] Erro ao criar registro a partir das tuplas\n\n");
            return -1;
          }
          
          // se tudo ok, coloca registro na fila
          AppendFila2(entradasList, record);
        }
      }
    }
    NextFila2(&tuplas);
    tuplaAtual = GetAtIteratorFila2(&tuplas);
  }
  return 0;
}

/*----------------------------------------------------------------------------------------------
Função que diz onde está no disco o próximo bloco do arquivo a ser acessado.

PS.: Provavelmente é uma boa usar como parâmetro o último bloco já acessado
----------------------------------------------------------------------------------------------*/
int getBlockFromMFT(/*mft_record registro, int blocoAtual*/){ //recebo VBN ou LBN?
	/*struct t2fs_4tupla tuplaAtual;
	int LBN_encontrado = -1;
	int dif;

	int i;
	for(i = 0; i < NUM_TUPLAS; i++){
	  tuplaAtual = registro.tuplas[i]; //testar se a tuplaAtual é valida antes
	  if(tuplaAtual.virtualBlockNumber <= blocoAtual){
	  	if((tuplaAtual.virtualBlockNumber + tuplaAtual.numberOfContiguosBlocks - 1) >= blocoAtual)
			dif = blocoAtual - tuplaAtual.virtualBlockNumber;
			LBN_encontrado = tuplaAtual.virtualBlockNumber + dif;
	  }
	}
	
	return LBN_encontrado;*/
}

/*----------------------------------------------------------------------------------------------
Função que lê o bloco de um determinado setor do disco para o buffer recebido com parâmetro.

Retorna: 0  --Se lido com sucesso
	 	-1  --Caso houve erro na leitura

GABRIEL JOB
----------------------------------------------------------------------------------------------*/
int readBlock(int sectorNumber, unsigned char blockBuffer[BLOCK_SIZE]){
	int i;
	int bytesLidos = 0;
	unsigned char sectorBuffer[SECTOR_SIZE];

	for(i=0; i<SECTORS_PER_BLOCK; i++){
		if(read_sector(sectorNumber + i, sectorBuffer) != 0)
			return -1;
		memcpy(blockBuffer + bytesLidos, sectorBuffer, SECTOR_SIZE);
		bytesLidos = bytesLidos + SECTOR_SIZE;
	}
	return 0;
}

/*----------------------------------------------------------------------------------------------
Função que escreve o bloco passado como parâmetro num determinado setor do disco.

Retorna: 0  --Se escrito com sucesso
		-1  --Caso houve falha na escrita

GABRIEL JOB
----------------------------------------------------------------------------------------------*/
int writeBlock(int sectorNumber, unsigned char blockBuffer[BLOCK_SIZE]){
	int i;
	int bytesEscritos = 0;
	unsigned char sectorBuffer[SECTOR_SIZE];
	
	for(i=0; i<SECTORS_PER_BLOCK; i++){
		memcpy(sectorBuffer, blockBuffer + bytesEscritos, SECTOR_SIZE);
		if(write_sector(sectorNumber + i, sectorBuffer) != 0)
			return -1;
		bytesEscritos = bytesEscritos + SECTOR_SIZE;
	}
	return 0;
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

int readRecord(int sector, int position_in_sector, struct t2fs_record* record) {
    unsigned char buffer_sector[SECTOR_SIZE];

    if (read_sector(sector, buffer_sector) != 0) {
        printf("[ERRO] Erro ao ler setor %i\n", sector);
        return -1;
    }

    // Lê registro
    memcpy(&record->TypeVal, &buffer_sector[position_in_sector * 64], sizeof(record->TypeVal));
    memcpy(&record->name, &buffer_sector[(position_in_sector * 64) + 1], sizeof(record->name));
    memcpy(&record->blocksFileSize, &buffer_sector[(position_in_sector * 64) + 52], sizeof(record->blocksFileSize));
    memcpy(&record->bytesFileSize, &buffer_sector[(position_in_sector * 64) + 56], sizeof(record->bytesFileSize));
    memcpy(&record->MFTNumber, &buffer_sector[(position_in_sector * 64) + 60], sizeof(record->MFTNumber));

    return 0;
}

// salva no ponteiro tupla o que for lido do disco
int readTupla(int sector, int position_in_sector, struct t2fs_4tupla* tupla) {
    unsigned char buffer_sector[SECTOR_SIZE];
    
    if (read_sector(sector, buffer_sector) != 0) {
        printf("[ERRO] Erro ao ler setor %i\n", sector);
        return -1;
    }
    
    BYTE t[TUPLA_SIZE];
    
    int i;
    for(i = 0; i < TUPLA_SIZE; i++) {
      t[i] = buffer_sector[(TUPLA_SIZE * position_in_sector) + i];
      printf("%i\n", t[i]);
    }

    return 0;
}

char *nameFromPath(char *pathname) {
  char *token;
  char str[MAX_FILE_NAME_SIZE];
  strcpy(str, pathname);
  
  token = strtok(str, "/");
  char *lastToken;
  while (token != NULL) {
    lastToken = token ;
    token = strtok (NULL, "/");
  }
  return lastToken;
}

char *pathDoPai(char *pathname) {
  char *token;
  char str[MAX_FILE_NAME_SIZE], pathDoPai[MAX_FILE_NAME_SIZE];
  
  strcpy(str, pathname);
  strcpy(pathDoPai, "");
  
  token = strtok(str, "/");
  int levels = 1;
  
  char *lastToken;
  while (token != NULL) {
    lastToken = token;
    token = strtok (NULL, "/");
    
    if (token != NULL) {
      strcat(pathDoPai, "/");
      strcat(pathDoPai, lastToken);
      levels += 1;
    }
  }
  if (levels <= 1) {
    strcpy(pathDoPai, "/");
  }
  printf("%s\n", pathDoPai);
  return (char *) pathDoPai;
}