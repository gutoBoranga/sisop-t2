#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <apidisk.h>
#include <bitmap2.h>
#include <t2fs.h>
#include <mft_manager.h>
#include <implementation.h>
#include <support.h>

struct t2fs_4tupla createTupla(DWORD atributeType, DWORD vbn, DWORD lbn, DWORD contiguosBlocks) {
  struct t2fs_4tupla tupla;
  
  tupla.atributeType = atributeType;
  tupla.virtualBlockNumber = vbn;
  tupla.logicalBlockNumber = lbn;
  tupla.numberOfContiguosBlocks = contiguosBlocks;
  
  return tupla;
}

di_node* createDInode(int i) {
  di_node *node = (di_node*) malloc(sizeof(di_node));
  
  node->index = i;
  node->next = NULL;
  
  return node;
}

di_node* appendDInode(di_node *node, di_node *head) {
  di_node *n = head;
  
  if (head == NULL) {
    return node;
  }
  
  while(n->next != NULL)  {
    n = n->next;
  }
  n->next = node;
  
  return head;
}

di_node* removeFirstDInode(di_node *head) {
  if (head == NULL) {
    return NULL;
  }
  
  di_node *second = head->next;
  free(head);
  
  return second;
}

void print_the_sound_of_a_capybara() {
  printf("\nAheoUYUOoomTgUU\n");
  
  // unsigned char buffer[SECTOR_SIZE];
  // char *p1 = strtok(NULL, " ");
  // if (p1==NULL) {
  //   printf ("Erro no comando.\n");
  // }
  // int sector = atoi(p1);
  // int error = read_sector (sector, buffer);
}

/*----------------------------------------------------------------------------------------------
Função para ler toda a área de MFT do disco para a memória.

Retorna: 0  --Se lida com sucesso
		-1  --Caso houve falha na leitura

GABRIEL JOB
----------------------------------------------------------------------------------------------*/
int boot_area_MFT (PFILA2 area_MFT){
	unsigned char blockBuffer[BLOCK_SIZE];
	
	if(readBlock(0, blockBuffer) != 0) return -1; //PEGA NO BLOCO DE BOOT
	int MFTBlocksSize = *((WORD *)(blockBuffer + 8)); // A QUANTIDADE DE BLOCOS DA AREA DE MFT

	struct 	t2fs_4tupla *tuplaAtual;
	int blocoAtualNumber, i, j, offset = 0;
	reg_MFT *novo_regMFT;	

	for(blocoAtualNumber = 1; blocoAtualNumber <= 1/*MFTBlocksSize*/; blocoAtualNumber++){
		if(readBlock(blocoAtualNumber,blockBuffer)  != 0) 
			return -1;

		offset = 0;
		for(i = 0; i < 2; i++){ //um bloco possui 2 conjuntos de 32 tuplas
			novo_regMFT = (reg_MFT *) malloc(sizeof(reg_MFT));
			CreateFila2(&novo_regMFT->tuplas);	
			
			for(j = 0; j < 32; j++){
				tuplaAtual = (struct t2fs_4tupla *) malloc(sizeof(struct t2fs_4tupla));

				tuplaAtual->atributeType = *((DWORD *) (blockBuffer + offset));
				tuplaAtual->virtualBlockNumber = *((DWORD *) (blockBuffer + offset + 4));
				tuplaAtual->logicalBlockNumber = *((DWORD *) (blockBuffer + offset + 8));
				tuplaAtual->numberOfContiguosBlocks = *((DWORD *) (blockBuffer + offset + 12));		
				
				offset += 16;				
				AppendFila2(&novo_regMFT->tuplas, tuplaAtual);
			}
			AppendFila2(area_MFT, &novo_regMFT->tuplas);
		}
	}
	return 0;
}

/*----------------------------------------------------------------------------------------------
Função para escrever toda a área de MFT da memória pro disco.

Retorna: 0  --Se escrita com sucesso
		-1  --Caso houve falha na escrita

GABRIEL JOB
----------------------------------------------------------------------------------------------*/
int write_area_MFT(PFILA2 area_MFT){
	unsigned char blockBuffer[BLOCK_SIZE];
	
	if(readBlock(0, blockBuffer) != 0) return -1; //PEGA NO BLOCO DE BOOT
	int MFTBlocksSize = *((WORD *)(blockBuffer + 8)); // A QUANTIDADE DE BLOCOS DA AREA DE MFT

	reg_MFT *regMFT;
	struct 	t2fs_4tupla *tuplaAtual;

	int blocoAtualNumber;
	int offset = 0;
	int i, j, k;
	FirstFila2(area_MFT);
	for(blocoAtualNumber = 1; blocoAtualNumber <= 1/*MFTBlocksSize*/; blocoAtualNumber++){
		offset = 0;
		
		for(i = 0; i < 2; i++){
			regMFT = (reg_MFT *) GetAtIteratorFila2(area_MFT);
			FirstFila2(&regMFT->tuplas);
				
			for(k = 0; k < 32; k++){
				tuplaAtual = (struct t2fs_4tupla *) GetAtIteratorFila2(&regMFT->tuplas);
				memcpy(blockBuffer + offset, tuplaAtual, 16);
				offset += 16;	
				NextFila2(&regMFT->tuplas);			
			}
			NextFila2(area_MFT);			
		}
		if(writeBlock(blocoAtualNumber, blockBuffer) != 0) return -1;	
	}
	return 0;
}

/*----------------------------------------------------------------------------------------------
Imprime toda a área de MFT na memória.

GABRIEL JOB
----------------------------------------------------------------------------------------------*/
void print_area_MFT(PFILA2 area_MFT){
	reg_MFT *regMFT;
	struct 	t2fs_4tupla *tuplaAtual;
	int mftNumber=0;

	if(FirstFila2(area_MFT) == 0){				
		do{
			regMFT = (reg_MFT *) GetAtIteratorFila2(area_MFT);
			if(FirstFila2(&regMFT->tuplas) == 0){
				printf("\n\n--------------Registro MFT: %d-------------------\n", mftNumber);
				do{
					tuplaAtual = (struct t2fs_4tupla *) GetAtIteratorFila2(&regMFT->tuplas);
					printf("\natributeType = %d", tuplaAtual->atributeType);
					printf("\nvirtualBlockNumber = %d", tuplaAtual->virtualBlockNumber);
					printf("\nlogicalBlockNumber = %d", tuplaAtual->logicalBlockNumber);
					printf("\nnumberOfContiguosBlocks = %d", tuplaAtual->numberOfContiguosBlocks);
				}while(NextFila2(&regMFT->tuplas) == 0);
				mftNumber++;
			}		
		}while(NextFila2(area_MFT) == 0);
	}
	else
		printf("\nAREA INVALIDA");
}
