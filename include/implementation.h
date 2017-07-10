#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <t2fs.h>
#include <support.h>

#define SECTOR_SIZE 256
#define SECTORS_PER_BLOCK 4
#define BLOCK_SIZE SECTOR_SIZE*SECTORS_PER_BLOCK

// structs

typedef struct t2fs_record registro_dir;

typedef struct diretorio {
    char* name;
    char* pai_pathname;
    int current_entry;
    int MTFNumber;
    int blockSize;
    int byteSize;
    FILA2 entradas;
} DIRETORIO;



// functions declaration

DIRETORIO* buscaDiretorioPai(char *pathname, int pathname_len, FILA2 dirList);
int readEntradas(int dirByteSize, PFILA2 entradasList);
int readBlock(int sectorNumber, unsigned char buffer[BLOCK_SIZE]);
int printaDiretoriosLista(FILA2 fila);
