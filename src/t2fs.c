#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <apidisk.h>
#include <bitmap2.h>
#include <t2fs.h>
#include <mft_manager.h>

// ===== Auxiliar functions declarations ====================================================================

void init_blocks();
void initBootBlock();
void initMFTBlock();

struct t2fs_record createRegister(BYTE typeVal, char name[MAX_FILE_NAME_SIZE], DWORD blocksSize, DWORD bytesSize, DWORD mftNumber);


// ===== Vars =============================================================================================

struct t2fs_bootBlock bootBlock;
struct t2fs_mftBlock mftBlock;

int has_initialized = 0;


// ===== API functions =====================================================================================

FILE2 create2 (char *filename) {
    if  (!has_initialized) {
      init_blocks();
      has_initialized = 1;
    }
}


int delete2 (char *filename) {
  init_blocks();
}


FILE2 open2 (char *filename) {
  
}


int close2 (FILE2 handle) {
  
}


int read2 (FILE2 handle, char *buffer, int size) {
  
}


int write2 (FILE2 handle, char *buffer, int size) {
  
}


int truncate2 (FILE2 handle) {
  
}


int seek2 (FILE2 handle, DWORD offset) {
  
}


int mkdir2 (char *pathname) {
  
}


int rmdir2 (char *pathname) {
  
}


DIR2 opendir2 (char *pathname) {
  
}


#define	END_OF_DIR	1

int readdir2 (DIR2 handle, DIRENT2 *dentry) {
  
}


int closedir2 (DIR2 handle) {
  
}


int identify2 (char *name, int size) {
  
  // função implementada em mft_manager.c apenas p/ testar a integração dos arquivos
  print_the_sound_of_a_capybara();
  
  if (strlen(name) <= size) {
    printf("\n%s\n", name);
    return 0;
  }
  return -1;
}


// ===== Auxiliar functions =====================================================================================

void init_blocks() {
  initBootBlock();
  initMFTBlock();
}

void initBootBlock() {
  strcpy(bootBlock.id, "T2fs");
  bootBlock.version = 0x7E11;
  bootBlock.blockSize = 0x0004;
  bootBlock.MFTBlocksSize = 0x0800;
  bootBlock.diskSectorSize = 0x8000;
  
  printf("\nfiz init no boot\n");
}

void initMFTBlock() {
  mftBlock.length = 0;
  
  // faz sentido setar os outros campos do registro de bitmap? não é um registro regular
  mftBlock.bitmapDescriptor.TypeVal = 0x00;
  
  mftBlock.rootDescriptor.TypeVal = 0x00;
  
  mftBlock.record2.TypeVal = 0x00;
  
  mftBlock.record3.TypeVal = 0x00;
  
  printf("\nfiz init no mft\n");
}

struct t2fs_record createRegister(BYTE typeVal, char name[MAX_FILE_NAME_SIZE], DWORD blocksSize, DWORD bytesSize, DWORD mftNumber) {
    struct t2fs_record new_register;
    
    new_register.TypeVal = typeVal;
    // strcpy(new_register.name, name);
    new_register.blocksFileSize = blocksSize;
    new_register.bytesFileSize = bytesSize;
    new_register.MFTNumber = mftNumber;
    
    return new_register;
}