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
	registro_dir *t2fs_reg;
	char* pai_pathname;
	int current_entry;
	FILA2 entradas;
} DIRETORIO;

typedef struct arquivo {
	FILE2 handle;
	registro_dir *t2fs_reg;
	char* pai_pathname;
	int current_pointer;
} ARQUIVO;

FILA2 dirList;
FILA2 arqList;

DIR2 last_dir_handle;
FILE2 last_arq_handle;

// functions declaration

DIRETORIO* buscaDiretorioPai(char *pathname, int pathname_len);
DIRETORIO* getDiretorio(DIR2 handle);
ARQUIVO* getArquivo(FILE2 handle);
registro_dir* getEntrada(DIRETORIO *dir);
registro_dir* get_t2fs_record_from_dir(DIRETORIO *dir, char *name);
// reg_MFT* busca_regMFT(int MFTNumber);
int busca_regMFT_livre();
int readEntradas2(FILA2 tuplas, PFILA2 entradasList);
int arquivo_ja_aberto(char *pathname);
int diretorio_ja_aberto(char *pathname);
int readBlock(int sectorNumber, unsigned char buffer[BLOCK_SIZE]);
int writeBlock(int sectorNumber, unsigned char blockBuffer[BLOCK_SIZE]);
int printaDiretoriosLista(FILA2 fila);
char *nameFromPath(char *pathname);
char *pathDoPai(char *pathname);
