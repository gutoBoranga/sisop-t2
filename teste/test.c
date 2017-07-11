#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <t2fs.h>
#include <support.h>
#include <implementation.h>
#include <mft_manager.h>

int main() {
  /*char *bla;
  //bla = "oi";
  char path[100];
  strcpy(path, "/a");
  bla = pathDoPai(path);
  printf("%s\n", bla);
  
  // printf("%s\n", nameFromPath("bla/capivara/verde"));
  // mkdir2("/a/");*/

	int resultado_create = create2("/teste");

	printf("\nResultado do create: %d\n", resultado_create);

	ARQUIVO *a;
	a = getArquivo(resultado_create);
	if(a > 0){
		printf("\nNome do arquivo criado: %s", a->t2fs_reg->name);
		printf("\nPai do arquivo criado: %s", a->pai_pathname);
		printf("\nPosisão no arquivo criado: %d", a->current_pointer);
		printf("\nTypeVal = %d", a->t2fs_reg->TypeVal);
		printf("\nblocksFileSize = %d", a->t2fs_reg->blocksFileSize);
		printf("\nbytesFileSize  = %d", a->t2fs_reg->bytesFileSize);
		printf("\nMFTNumber  = %d", a->t2fs_reg->MFTNumber);
		printf("\n\n");
	}
  
  int resultado_mkdir = mkdir2("/capivara");;
	printf("\nResultado do mkdir: %d\n", resultado_mkdir);

	DIRETORIO *d;
	d = getDiretorio(resultado_mkdir);
	if(d > 0){
		printf("\nNome do dir criado: %s", d->t2fs_reg->name);
		printf("\nPai do arquivo criado: %s", d->pai_pathname);
		printf("\nPosisão no arquivo criado: %d", d->current_entry);
		printf("\nTypeVal = %d", d->t2fs_reg->TypeVal);
		printf("\nblocksFileSize = %d", d->t2fs_reg->blocksFileSize);
		printf("\nbytesFileSize  = %d", d->t2fs_reg->bytesFileSize);
		printf("\nMFTNumber  = %d", d->t2fs_reg->MFTNumber);
		printf("\n\n");
	}
  
  int resultado_mkdir_de_novo = mkdir2("/capivara/verde");;
	printf("\nResultado do mkdir de novo: %d\n", resultado_mkdir_de_novo);

	DIRETORIO *y;
	y = getDiretorio(resultado_mkdir_de_novo);
	if(y > 0){
		printf("\nNome do dir criado: %s", y->t2fs_reg->name);
		printf("\nPai do arquivo criado: %s", y->pai_pathname);
		printf("\nPosisão no arquivo criado: %d", y->current_entry);
		printf("\nTypeVal = %d", y->t2fs_reg->TypeVal);
		printf("\nblocksFileSize = %d", y->t2fs_reg->blocksFileSize);
		printf("\nbytesFileSize  = %d", y->t2fs_reg->bytesFileSize);
		printf("\nMFTNumber  = %d", y->t2fs_reg->MFTNumber);
		printf("\n\n");
	}

	return 0;
}
