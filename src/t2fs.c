#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <apidisk.h>
#include <bitmap2.h>
#include <t2fs.h>
#include <mft_manager.h>
#include <implementation.h>

// ===== Auxiliar functions declarations ====================================================================

void init_blocks();
void initBootBlock();
void initMFTBlock();

struct t2fs_record *createRegister(BYTE typeVal, char name[MAX_FILE_NAME_SIZE], DWORD blocksSize, DWORD bytesSize, DWORD mftNumber);
int path_is_valid(char *pathname, DWORD typeVal);
int pathsAreEqual(char path1[MAX_FILE_NAME_SIZE], char path2[MAX_FILE_NAME_SIZE]);

// ===== Vars =============================================================================================

struct t2fs_bootBlock bootBlock;
struct t2fs_mftBlock mftBlock;

int has_initialized = 0;
int mft_filesDescriptors_max_size = MFT_BLOCK_SIZE - NUM_SPECIAL_RECORDS;

// ===== API functions =====================================================================================

FILE2 create2 (char *filename) {
  if  (!has_initialized) {
    init_blocks();
    has_initialized = 1;
  }
}


int delete2 (char *filename) {
  if  (!has_initialized) {
    init_blocks();
    has_initialized = 1;
  }
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
  if (!has_initialized) {
    init_blocks();
    has_initialized = 1;
  }
  
  if (!path_is_valid(pathname, TYPEVAL_DIRETORIO)) {
    return -1;
  }
  
  // cria novo registro t2fs
  struct t2fs_record *new_record;
  
  new_record = createRegister(TYPEVAL_DIRETORIO, pathname, 1, 0, mftBlock.next_valid_MFTnumber);
  
  // salvar no disco
  
  
  // cria novo mft_record no mft_manager
  mft_record *mft_rec;
  mft_rec = malloc(sizeof(mft_record));
  mft_rec->record = new_record;
  
  mft_rec->tuplas[0] = createTupla(0, 0, 0, 0); // tem que ver estes parametros
  
  // adiciona no array de registros do mtfBlock
  int index = mftBlock.next_valid_MFTnumber - NUM_SPECIAL_RECORDS;
  mftBlock.filesDescriptors[index] = mft_rec;
  
  mftBlock.next_valid_MFTnumber ++;
  
  printf("---------- Novo registro no MFT ----------\n");
  printf("| Nome: %s\n", mftBlock.filesDescriptors[index]->record->name);
  printf("| MFTNumber: %i\n", mftBlock.filesDescriptors[index]->record->MFTNumber);
  printf("------------------------------------------\n\n");
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
}

void initMFTBlock() {
  mftBlock.next_valid_MFTnumber = 0;
  
  // cria registro t2fs para o bitmap
  struct t2fs_record *bitmap;
  bitmap = createRegister(TYPEVAL_INVALIDO, "", 0, 0, mftBlock.next_valid_MFTnumber);
  
  // cria registro mft para representar o bitmap
  mftBlock.bitmapDescriptor.record = bitmap;
  mftBlock.bitmapDescriptor.tuplas[0] = createTupla(0, 0, 0, 0);
  
  mftBlock.next_valid_MFTnumber += 1;
  
  // cria registro t2fs do dir root
  struct t2fs_record *root;
  root = createRegister(TYPEVAL_DIRETORIO, "/", 1, 0, mftBlock.next_valid_MFTnumber);
  
  
  
  // tem que salvar no disco os registros t2fs_record
  
  
  
  mftBlock.next_valid_MFTnumber += 1;

  // cria registro mft para representar o root
  //// seta seu record para apontar pro registro t2fs que criamos acima
  mftBlock.rootDescriptor.record = root;
  
  //// seta a primeira tupla do array pra representar que está vazio
  mftBlock.rootDescriptor.tuplas[0] = createTupla(0, 0, 0, 0);
  
  // cria os 2 registros mft "reservados para uso futuro"
  struct t2fs_record invalid_record;
  invalid_record.TypeVal = TYPEVAL_INVALIDO;
  
  mftBlock.descriptor2.record = malloc(sizeof(struct t2fs_record));
  mftBlock.descriptor2.record = &invalid_record;
  (mftBlock.descriptor2.record)->MFTNumber = mftBlock.next_valid_MFTnumber;
  mftBlock.next_valid_MFTnumber += 1;
  
  mftBlock.descriptor3.record = malloc(sizeof(struct t2fs_record));
  mftBlock.descriptor3.record = &invalid_record;
  (mftBlock.descriptor3.record)->MFTNumber = mftBlock.next_valid_MFTnumber;
  mftBlock.next_valid_MFTnumber += 1;
  
  // malloca a parte do mft que vai conter todos os registros restantes
  mftBlock.filesDescriptors = malloc(sizeof(mft_record) * mft_filesDescriptors_max_size);
}

struct t2fs_record *createRegister(BYTE typeVal, char name[MAX_FILE_NAME_SIZE], DWORD blocksSize, DWORD bytesSize, DWORD mftNumber) {
    struct t2fs_record *new_register;
    new_register = malloc(sizeof(struct t2fs_record));
    
    new_register->TypeVal = typeVal;
    strcpy(new_register->name, name);
    new_register->blocksFileSize = blocksSize;
    new_register->bytesFileSize = bytesSize;
    new_register->MFTNumber = mftNumber;
    
    return new_register;
}

int path_is_valid(char *pathname, DWORD typeVal) {
  int current_max = mftBlock.next_valid_MFTnumber - NUM_SPECIAL_RECORDS;
    
  char father[strlen(pathname)], c;
  int i = 0;
  
  // garante que começa no root
  c = pathname[0];
  if (c =! '/') {
    printf("[ERRO] O path %s não começa com '/\'\n\n", pathname);
    return 0;
  }
  
  strcpy(father, "/");
  
  for (i = 1; i < strlen(pathname) - 1; i ++) {
    c = pathname[i];
    
    // se for o '/', copia pro father tudo que tem antes do '/'
    if (c == '/') {
      strncpy(father, pathname, i);
      father[i] = '\0';
    }
  }
  
  // confere se há algum item no filesDescriptors
  if ((mftBlock.next_valid_MFTnumber - NUM_SPECIAL_RECORDS) > 0) {
    int i, dir_pai_existe;
    i = dir_pai_existe = 0;
    
    // se o dir father for o root '/', marca que flag de existência do sir pai
    if (strcmp(father, "/") == 0) {
      dir_pai_existe = 1;
    
    // senão, coloca um '/' no fim do path
    } else {
      strcat(father, "/");
    }
    
    while(i < current_max) {
      // confere se já existe arquivo com aquele path
      char descriptor_barra[strlen(mftBlock.filesDescriptors[i]->record->name) + 1], pathname_barra[strlen(pathname) + 1];
      int pathsEqual = 0;
      
      // prepara possibilidades para comparar:
      strcpy(pathname_barra, pathname);
      strcat(pathname_barra, "/");
  
      strcpy(descriptor_barra, mftBlock.filesDescriptors[i]->record->name);
      strcat(descriptor_barra, "/");
      
      //
      // Exemplo: pathname (que o cara quer criar) é "/a"
      //          descritor (que está em mft[i]) é   "/a/"
      //
      // Possibilidades:
      // Original:          /a  vs /a/
      // Pathname + '/':    /a/ vs /a/   <= AQUI VAI DESCOBRIR QUE TEM MESMO PATH
      // Descritor + '/':   /a  vs /a//
      //
      pathsEqual += pathsAreEqual(pathname, mftBlock.filesDescriptors[i]->record->name);
      pathsEqual += pathsAreEqual(pathname_barra, mftBlock.filesDescriptors[i]->record->name);
      pathsEqual += pathsAreEqual(pathname, descriptor_barra);
      
      // se somou 1 em algum dos anteriores, path já existe. Retorna "false"
      if (pathsEqual) {
        printf("[ERRO] Já existe um arquivo com o path %s\n\n", pathname);
        return 0;
      }
      
      // confere se o diretório pai existe
      if(strcmp(father, mftBlock.filesDescriptors[i]->record->name) == 0) {
        dir_pai_existe = 1;
      }
      
      i++;
    }
    // se não achou o diretório pai, retorna "false"
    if (!dir_pai_existe) {
      printf("[ERRO] Não existe estrutura de diretório %s\n\n", father);
      return 0;
    }
    
  // caso seja o primeiro diretório a ser criado, o pai só pode ser o root
  } else {
    if (strcmp(father, "/") != 0) {
      printf("[ERRO] Não existe estrutura de diretório %s\n\n", father);
      return 0;
    }
  }
  return 1;
}

int pathsAreEqual(char path1[MAX_FILE_NAME_SIZE], char path2[MAX_FILE_NAME_SIZE]) {
  if (strcmp(path1, path2) == 0) {
    return 1;
  }
  return 0;
}