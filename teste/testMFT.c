#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <t2fs.h>
#include <support.h>
#include <implementation.h>
#include <mft_manager.h>

void testeTuplas() {
	struct t2fs_4tupla *tupla;
	// readTupla(5, 0, tupla);
	// unsigned char block[BLOCK_SIZE];
	// if(readBlock(0, block) != 0)
	// 	return -1;
	//
	// int i;
	// for (i = 0; i < SECTOR_SIZE; i ++) {
	// 	printf("%i: %c\n", i, block[i]);
	// }
	// unsigned char blockBuffer[BLOCK_SIZE];
	unsigned char sectorBuffer[SECTOR_SIZE];

	if(read_sector(0, sectorBuffer) != 0) {
		printf("erro\n");
	}
		
	int i;
	for (i = 0; i < SECTOR_SIZE; i ++) {
		// snprintf(sectorBuffer[i],5,"%04x",255);
		printf("%i: %i\n", i, sectorBuffer[i]);
	}
	
	// memcpy(blockBuffer, sectorBuffer, SECTOR_SIZE);
	// bytesLidos = bytesLidos + SECTOR_SIZE;
	
	
	// printf("AtributeType: %i\n", tupla->atributeType);
	// printf("AtributeType: %i\n", tupla->virtualBlockNumber);
	// printf("AtributeType: %i\n", tupla->logicalBlockNumber);
	// printf("AtributeType: %i\n", tupla->numberOfContiguosBlocks);
}

int main(){
	// unsigned char sectorBuffer[SECTOR_SIZE];
	// unsigned char id[0];
	//
	// if(read_sector(1, sectorBuffer) != 0) {
	// 	printf("erro\n");
	// }
	//
	// int i;
	// // lê id
	// for (i = 0; i < SECTOR_SIZE; i ++) {
	// 	BYTE byteLido = sectorBuffer[i];
	// 	printf("%d\n", byteLido);
	// }
	
	struct t2fs_4tupla* tupla;
	
	// readTupla(6, 0, tupla);
	
	if(CreateFila2(&area_MFT) == 0){
		boot_area_MFT(&area_MFT);
		mkdir2("/a");
		// print_area_MFT(&area_MFT);
		// write_area_MFT(&area_MFT);
		// if(CreateFila2(&areaMFT_2) == 0){
		// 	boot_area_MFT(&areaMFT_2);
		// 	print_area_MFT(&areaMFT_2);
		// }
	}
	
	return 0;
}
