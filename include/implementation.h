#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <t2fs.h>
#include <support.h>

// tamanho em bytes
#define RECORDS_PER_SECTOR 4
#define SECTORS_PER_BLOCK 4

#define SECTOR_SIZE 256
#define RECORD_SIZE 64
#define TUPLA_SIZE 16
#define BLOCK_SIZE SECTOR_SIZE*SECTORS_PER_BLOCK



// structs

typedef struct t2fs_record registro_dir;

typedef struct diretorio {
	DIR2 handle;
	char* name;
	char* pai_pathname;
	int current_entry;
	int MTFNumber;
	int blockSize;
	int byteSize;
	FILA2 entradas;
} DIRETORIO;

typedef struct arquivo {
	FILE2 handle;
	char* name;
	char* pai_pathname;
	int current_pointer;
	int MFTNumber;
	int blockSize;
	int byteSize;
} ARQUIVO;

FILA2 dirList;


// functions declaration

DIRETORIO* buscaDiretorioPai(char *pathname, int pathname_len);
DIRETORIO* getDiretorio(DIR2 handle);
registro_dir* getEntrada(DIRETORIO *dir);
int busca_regMFT_livre();
int readEntradas(int dirByteSize, PFILA2 entradasList);
int readBlock(int sectorNumber, unsigned char buffer[BLOCK_SIZE]);
int writeBlock(int sectorNumber, unsigned char blockBuffer[BLOCK_SIZE]);
int printaDiretoriosLista(FILA2 fila);
