#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <t2fs.h>
#include <support.h>
#include <implementation.h>
#include <mft_manager.h>

int main(){
	/*unsigned char blocoLido[BLOCK_SIZE];

	if(readBlock(0, blocoLido) == 0){
		printf("\nLEITURA DO DISCO COM SUCESSO");
		struct t2fs_bootBlock dadosBoot;

		strncpy(dadosBoot.id, blocoLido, 4);
		dadosBoot.version = *((WORD *)(blocoLido + 4));
		dadosBoot.blockSize = *((WORD *)(blocoLido + 6));
		dadosBoot.MFTBlocksSize = *((WORD *)(blocoLido + 8));
		dadosBoot.diskSectorSize = *((DWORD *)(blocoLido + 10));

		printf("\n------------BLOCO DE BOOT------------");
		printf("\nid: %s", dadosBoot.id);
		printf("\nversion: %d", dadosBoot.version);
		printf("\nblockSize: %d", dadosBoot.blockSize);
		printf("\nMFTBlocksSize: %d", dadosBoot.MFTBlocksSize);
		printf("\ndiskSectorSize: %d", dadosBoot.diskSectorSize);
	
	}*/
	

	FILA2 areaMFT;
	FILA2 areaMFT_2;

	if(CreateFila2(&area_MFT) == 0){
		boot_area_MFT(&area_MFT);
		print_area_MFT(&area_MFT);
		write_area_MFT(&area_MFT);
		if(CreateFila2(&areaMFT_2) == 0){
			boot_area_MFT(&areaMFT_2);
			print_area_MFT(&areaMFT_2);
		}
	}
	return 0;
}
