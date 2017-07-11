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

struct t2fs_record *createRegister(BYTE typeVal, char *name, DWORD blocksSize, DWORD bytesSize, DWORD mftNumber);
int path_is_valid(char *pathname, DWORD typeVal);
int stringCompare(char str1[MAX_FILE_NAME_SIZE], char str2[MAX_FILE_NAME_SIZE]);
int pathsAreEquivalent(char path1[MAX_FILE_NAME_SIZE], char path2[MAX_FILE_NAME_SIZE]);
mft_record *getMFTRecord(char *pathname);
void printDeletedIndexesList();


// ===== Vars =============================================================================================

struct t2fs_bootBlock bootBlock;
struct t2fs_mftBlock mftBlock;

int has_initialized = 0;
int mft_filesDescriptors_max_size = MFT_BLOCK_SIZE - NUM_SPECIAL_RECORDS;

// ===== API functions =====================================================================================

FILE2 create2 (char *filename) {
  if (!has_initialized) {
		init_blocks();
		has_initialized = 1;
	}
 
	if (!path_is_valid(filename, TYPEVAL_REGULAR)) {
		printf("[ERRO] Path %s inválido\n\n", filename);
		return -1;
	}

	DIRETORIO *dirPai;
	dirPai = buscaDiretorioPai(filename, strlen(filename));
	if(dirPai == NULL)
		return -1;

	ARQUIVO *novo_arquivo;
	novo_arquivo = (ARQUIVO *) malloc(sizeof(ARQUIVO));

	//função que pega só o nome do filename
	//função que pega tudo menos o nome do file name

	registro_dir *t2fs_reg;	//cria um novo t2fs_record
	t2fs_reg->TypeVal = 1;
	t2fs_reg->blocksFileSize = 0;
	t2fs_reg->bytesFileSize = 0;
	t2fs_reg->MTFNumber = busca_regMFT_livre();
	
	AppendFila2(&dirPai->entradas, t2fs_reg);
	novo_arquivo->t2fs_reg = t2f2_reg;

	novo_arquivo->current_pointer = 0;

	last_arq_handle++;
	novo_arquivo->handle = last_arq_handle;

	AppendFila2(&arqList, novo_arquivo);

	return novo_arquivo->handle;	
}


int delete2 (char *filename) {
  if (!path_is_valid(filename, TYPEVAL_DIRETORIO)) {
    printf("[ERRO] Path %s inválido\n\n", filename);
    return -1;
  }
  
  int current_max = mftBlock.next_valid_MFTnumber - NUM_SPECIAL_RECORDS;
  
  // se não existe nenhum registro
  if (current_max <= 0) {
    return -1;
  }
  
  char descriptor[MAX_FILE_NAME_SIZE];
  int i, found;
  i = found = 0;
  
  mft_record *mft_rec;
  
  while(i < current_max) {
    strncpy(descriptor, mftBlock.filesDescriptors[i]->record->name, strlen(filename));
    
    // se encontrou o registro
    if (pathsAreEquivalent(filename, mftBlock.filesDescriptors[i]->record->name)) {
      // E É UM ARQUIVO REGULAR UHUUL
      if (mftBlock.filesDescriptors[i]->record->TypeVal == TYPEVAL_REGULAR) {
        mft_rec = mftBlock.filesDescriptors[i];
        found = 1;
      
      // caso não seja um arquivo regular
      } else {
        printf("[ERRO] Path %s não é um arquivo regular\n\n", filename);
        return -1;
      }
    }
    i++;
  }
  
  if (!found) {
    printf("[ERRO] Arquivo %s não foi encontrado\n\n", filename);
    return -1;
  }
  
  // seta flag do registro como inválido
  mft_rec->valid = MFT_RECORD_INVALID;
  
  // cria nodo pra lista de indices removidos
  di_node *node = createDInode(mft_rec->record->MFTNumber);
  mftBlock.deleted_indexes = appendDInode(node, mftBlock.deleted_indexes);
  
  printf("[INFO] Path %s removido com sucesso!\n\n", filename);
  
  return 0;
}


FILE2 open2 (char *filename) {
  int current_max = mftBlock.next_valid_MFTnumber - NUM_SPECIAL_RECORDS;
  int i = 0;
  
  // se já existe algum registro
  if ((mftBlock.next_valid_MFTnumber - NUM_SPECIAL_RECORDS) > 0) {
    printf("Lista de registros no MFT:\n\n");
    printf("------------------------------------\n");
    while(i < current_max) {
      if(mftBlock.filesDescriptors[i]->valid == MFT_RECORD_VALID) {
        printf("| Path: %s\n", mftBlock.filesDescriptors[i]->record->name);
        printf("| MFT: %i\n", mftBlock.filesDescriptors[i]->record->MFTNumber);
        printf("------------------------------------\n");
      }
      i++;
    }
  }
}


int close2 (FILE2 handle) {
  if (!has_initialized) {
    init_blocks();
    has_initialized = 1;
  }
}


int read2 (FILE2 handle, char *buffer, int size) {
  
}


int write2 (FILE2 handle, char *buffer, int size) {
  
}


int truncate2 (FILE2 handle) {
  printf("\n%i\n",pathsAreEquivalent("/a/b/", "/a/b/"));
  printf("%i\n",pathsAreEquivalent("/a/b", "/a/b/"));
  printf("%i\n",pathsAreEquivalent("/a/b/", "/a/b"));
  printf("%i\n",pathsAreEquivalent("/a/b", "/a/b"));
  printf("%i\n",pathsAreEquivalent("/a/b/", "/a/"));
}


int seek2 (FILE2 handle, DWORD offset) {
  
}


int mkdir2 (char *pathname) {
  if (!has_initialized) {
    init_blocks();
    has_initialized = 1;
  }
  
  if (!path_is_valid(pathname, TYPEVAL_DIRETORIO)) {
    printf("[ERRO] Path %s inválido\n\n", pathname);
    return -1;
  }
  
  // pega registro do root no mft
  FirstFila2(&area_MFT);
  NextFila2(&area_MFT);
  
  reg_MFT *dir;
  dir = (reg_MFT *) malloc(sizeof(reg_MFT));
  dir = GetAtIteratorFila2(&area_MFT);
  
  // pega entradas
  FILA2 entradas;
  CreateFila2(&entradas);
  
  char *token;
  char str[MAX_FILE_NAME_SIZE];
  strcpy(str, pathname);
  
  token = strtok(str, "/");
  char *lastToken;
  
  int should_tokenize = 0;
  
  // vai percorrer os dirs intermediarios
  while (token != NULL) {
    lastToken = token;
    should_tokenize = 1;
    
    // lê suas entradas
    readEntradas2(dir->tuplas, &entradas);
    FirstFila2(&entradas);
    
    // pega o primeiro registro (se houver)
    struct t2fs_record *record;
    record = malloc(sizeof(struct t2fs_record));
    record = GetAtIteratorFila2(&entradas);
    
    // percorre todas entradas procurando se tem algum arquivo com mesmo nome do que quero criar
    while (record != NULL) {
      // se o nome da entrada for o mesmo do diretório que está sendo percorrido atualmente
      if (strcmp(record->name, lastToken) == 0) {
        dir = busca_regMFT(record->MFTNumber);
        if (dir == NULL) {
          printf("[ERRO] Não foi possível achar diretórios intermediários.\n");
          return -1;
        }
      }
      
      // se o nome da entrada for o mesmo que o nome do dir que quero criar
      if (strcmp(record->name, nameFromPath(pathname)) == 0) {
        if (should_tokenize) {
          token = strtok (NULL, "/");
          should_tokenize = 0;
        }
        
        // se token = NULL, quer dizer que chegou no último dir do path dado.
        // ou seja, se já tem uma entrada com o mesmo nome do dir que quero criar, não pode criar!
        if (token == NULL) {
          printf("[ERRO] Já existe um diretorio com este path\n\n");
          return -1;
        }
      }
      
      NextFila2(&entradas);
      record = GetAtIteratorFila2(&entradas);
    }
    if(should_tokenize) {
      token = strtok (NULL, "/");
      should_tokenize = 0;
    }
  }
  
  // se chegou até aqui, pode criar um registro sem medo
  struct t2fs_record *new_record;
  new_record = malloc(sizeof(struct t2fs_record));
  new_record = createRegister(TYPEVAL_DIRETORIO, nameFromPath(pathname), 0, 0, 0);
  
  // cria novo registro do mft para este dir
  reg_MFT *novo_regMFT;
	novo_regMFT = (reg_MFT *) malloc(sizeof(reg_MFT));
  
	CreateFila2(&novo_regMFT->tuplas);
  
  struct 	t2fs_4tupla *tuplaAtual;
  
  // cria as 32 tuplas do registro
  int i;
  for(i = 0; i < 32; i++) {
    tuplaAtual = createTuplaPointer(0, 0, 0, 0);
    AppendFila2(&novo_regMFT->tuplas, tuplaAtual);
  }
  
  int mft_livre = busca_regMFT_livre();
  printf("mft: %i\n", mft_livre);
  
  // ADICIONAR O REG_MFT NA FILA/LISTA DE ENTRADAS DO DIRETORIO PAI
    
  return 0;
}


int rmdir2 (char *pathname) {
  if (!path_is_valid(pathname, TYPEVAL_DIRETORIO)) {
    printf("[ERRO] Path %s inválido\n\n", pathname);
    return -1;
  }
  
  int current_max = mftBlock.next_valid_MFTnumber - NUM_SPECIAL_RECORDS;
  
  // se não existe nenhum registro
  if (current_max <= 0) {
    return -1;
  }
  
  char descriptor[MAX_FILE_NAME_SIZE];
  int i, number_of_matches;
  i = number_of_matches = 0;
  
  mft_record *mft_rec;
  
  while(i < current_max) {
    strncpy(descriptor, mftBlock.filesDescriptors[i]->record->name, strlen(pathname));
    
    // vê se encontrou o diretório em questão
    if (pathsAreEquivalent(pathname, mftBlock.filesDescriptors[i]->record->name)) {
      if (mftBlock.filesDescriptors[i]->record->TypeVal != TYPEVAL_DIRETORIO) {
        printf("[ERRO] Path %s não é um diretório\n\n", pathname);
        return -1;
      // encontrou o registro E É UM DIRETORIO UHUUL
      } else {
        mft_rec = mftBlock.filesDescriptors[i];
        number_of_matches += 1;
      }
    
    // vê se encontra algum subdiretório
    } else if (pathsAreEquivalent(pathname, descriptor)) {
      number_of_matches += 1;
    }
    i++;
  }
  
  if (number_of_matches == 0) {
    printf("[ERRO] Path %s não corresponde a um diretório\n\n", pathname);
    return -1;
  } else if (number_of_matches > 1) {
    printf("[ERRO] Diretório %s não pode ser removido pois não está vazio\n\n", pathname);
    return -1;
  }
  
  // seta flag do registro como inválido
  mft_rec->valid = MFT_RECORD_INVALID;
  
  // cria nodo pra lista de indices removidos
  di_node *node = createDInode(mft_rec->record->MFTNumber);
  mftBlock.deleted_indexes = appendDInode(node, mftBlock.deleted_indexes);
  
  return 0;
}


DIR2 opendir2 (char *pathname) {
  
}


#define	END_OF_DIR	1

int readdir2 (DIR2 handle, DIRENT2 *dentry) {
	DIRETORIO *dir = getDiretorio(handle);
	if(dir == NULL)
		return -2;

	registro_dir *entrada = getEntrada(dir);
	if(entrada == NULL)
		return -END_OF_DIR;

	strncpy(dentry->name, entrada->name, 51);
	dentry->fileType = entrada->TypeVal;
	dentry->fileSize = entrada->bytesFileSize;
	
	return 0;
}


int closedir2 (DIR2 handle) {
  di_node *head = createDInode(3);
  
  di_node *n1 = createDInode(6);
  appendDInode(n1, head);
  
  di_node *n2 = createDInode(10);
  appendDInode(n2, head);
  
  di_node *n3 = createDInode(1);
  appendDInode(n3, head);
  
  di_node *n = head;
  while (n->next != NULL) {
    printf("%i\n", n->index);
    n = n->next;
  }
  printf("%i\n\n\n", n->index);
  
  head = removeFirstDInode(head);
  n = head;
  if (head != NULL) {
    while (n->next != NULL) {
      printf("%i\n", n->index);
      n = n->next;
    }
    printf("%i\n\n\n", n->index);
  }
}


int identify2 (char *name, int size) {
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
	FILA2 areaMFT_2;

	if(CreateFila2(&area_MFT) == 0){
		boot_area_MFT();
  }
  
  
  mftBlock.next_valid_MFTnumber = 0;
  
  mftBlock.deleted_indexes = NULL;
  
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

struct t2fs_record *createRegister(BYTE typeVal, char *name, DWORD blocksSize, DWORD bytesSize, DWORD mftNumber) {
    struct t2fs_record *new_register;
    new_register = malloc(sizeof(struct t2fs_record));
    
    new_register->TypeVal = typeVal;
    strcpy(new_register->name, name);
    new_register->blocksFileSize = blocksSize;
    new_register->bytesFileSize = bytesSize;
    new_register->MFTNumber = mftNumber;
    
    return new_register;
}

// vale lembrar que aqui retorno 0 significa falso e 1 verdadeiro
int path_is_valid(char *pathname, DWORD typeVal) {
  char c;
  int i = 0;
  
  // garante que começa no root
  c = pathname[0];
  if (c != '/') {
    return 0;
  }
  
  // garante que não quer criar o root
  char *token;
  char str[MAX_FILE_NAME_SIZE];
  strcpy(str, pathname);
  
  token = strtok(str, "/");
  if (token == NULL) {
    return 0;
  }
    
  // se for arquivo regular, garante que path não termina em '/'
  if (typeVal == TYPEVAL_REGULAR) {
    c = pathname[strlen(pathname) - 1];
    if (c == '/') {
      return 0;
    }
  }
  
  return 1;
}
  
int father_exists(char *pathname) {
  int current_max = mftBlock.next_valid_MFTnumber - NUM_SPECIAL_RECORDS;
  char father[strlen(pathname)], c;
  int i = 0;
  
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
    int i, pathsEqual;
    i = pathsEqual = 0;
    
    // se o dir father for o root '/', marca que flag de existência do dir pai
    if (strcmp(father, "/") == 0) {
      return 1;
    }
    
    while(i < current_max) {
      int result = pathsAreEquivalent(father, mftBlock.filesDescriptors[i]->record->name);
      
      // se achou um path igual ao do father E for um diretorio, retorna true
      if (mftBlock.filesDescriptors[i]->record->TypeVal == TYPEVAL_DIRETORIO && mftBlock.filesDescriptors[i]->valid == MFT_RECORD_VALID && result > 0) {
        return 1;
      }
      
      i++;
    }
    printf("[ERRO] Não existe estrutura de diretório %s\n\n", father);
    return 0;
  
  // caso seja o primeiro diretório a ser criado, o pai só pode ser o root
  } else {
    if (strcmp(father, "/") != 0) {
      printf("[ERRO] Não existe estrutura de diretório %s\n\n", father);
      return 0;
    }
  }
  return 1;
}

int path_already_exists(char *pathname) {
  int current_max = mftBlock.next_valid_MFTnumber - NUM_SPECIAL_RECORDS;
  int i = 0;
  
  // se já existe algum registro
  if ((mftBlock.next_valid_MFTnumber - NUM_SPECIAL_RECORDS) > 0) {
    int pathsEqual = 0;
    
    while(i < current_max) {
      // confere se já existe arquivo com aquele path
      if (pathsAreEquivalent(pathname, mftBlock.filesDescriptors[i]->record->name)) {
        return 1;
      }
      i++;
    }
  }
  return 0;
}

int pathsAreEquivalent(char path1[MAX_FILE_NAME_SIZE], char path2[MAX_FILE_NAME_SIZE]) {
  char path1_barra[MAX_FILE_NAME_SIZE], path2_barra[MAX_FILE_NAME_SIZE];
  
  strcpy(path1_barra, path1);
  strcat(path1_barra, "/");

  strcpy(path2_barra, path2);
  strcat(path2_barra, "/");
  
  int pathsEqual = 0;
  pathsEqual += stringCompare(path1, path2);
  pathsEqual += stringCompare(path1_barra, path2);
  pathsEqual += stringCompare(path1, path2_barra);
  
  return pathsEqual;
}

int stringCompare(char str1[MAX_FILE_NAME_SIZE], char str2[MAX_FILE_NAME_SIZE]) {
  if (strcmp(str1, str2) == 0) {
    return 1;
  }
  return 0;
}

mft_record *getMFTRecord(char *pathname) {
  int current_max = mftBlock.next_valid_MFTnumber - NUM_SPECIAL_RECORDS;
  int pathsEqual = 0;
  int i = 0;
  mft_record *pointer;
  
  // se já existe algum registro
  if ((mftBlock.next_valid_MFTnumber - NUM_SPECIAL_RECORDS) > 0) {
    while(i < current_max) {
      if (pathsAreEquivalent(pathname, mftBlock.filesDescriptors[i]->record->name)) {
        pointer = mftBlock.filesDescriptors[i];
        return pointer;
      }
      i++;
    }
  }
  return pointer;
}

void printDeletedIndexesList() {
  di_node *n;
  n = mftBlock.deleted_indexes;
  
  printf("\nLista de indices removidos:");
  if (n != NULL) {
    while (n->next != NULL) {
      printf("\n%i", n->index);
      n = n->next;
    }
    printf("\n%i\n\n", n->index);
  } else {
    printf("\nNULL\n");
  }
}