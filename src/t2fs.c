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
  
  if (!father_exists(filename, TYPEVAL_REGULAR)) {
    return -1;
  }
  
  if (path_already_exists(filename)) {
    printf("[ERRO] Já existe um arquivo com o path %s\n\n", filename);
    return -1;
  }
  
  int mftNumber;
  int index;
  
  // se houver algum indice deletado na lista, pega o indice dele pro novo registro
  if (mftBlock.deleted_indexes != NULL) {
    index = mftBlock.deleted_indexes->index - NUM_SPECIAL_RECORDS;
    mftNumber = mftBlock.filesDescriptors[index]->record->MFTNumber;
    
    // libera mem do registro anteriormente naquele indice
    free(mftBlock.filesDescriptors[index]);
    //
    // talvez tenha que desalocar tudo que o antigo registro apontava. não sei
    //
    
    // e remove da lista
    mftBlock.deleted_indexes = removeFirstDInode(mftBlock.deleted_indexes);
    
  // se não houver nenhum registro removido pra reaproveitar
  } else {
    mftNumber = mftBlock.next_valid_MFTnumber;
    index = mftBlock.next_valid_MFTnumber - NUM_SPECIAL_RECORDS;
    
    mftBlock.next_valid_MFTnumber ++;
  }
  
  // cria novo registro t2fs
  struct t2fs_record *new_record;
  new_record = createRegister(TYPEVAL_REGULAR, filename, 1, 0, mftNumber);
  
  // salvar no disco
  
  
  // cria novo mft_record no mft_manager
  mft_record *mft_rec;
  mft_rec = malloc(sizeof(mft_record));
  mft_rec->record = new_record;
  mft_rec->valid = MFT_RECORD_VALID;
  mft_rec->tuplas[0] = createTupla(0, 0, 0, 0); // tem que ver estes parametros
  
  // adiciona no array de registros do mtfBlock
  mftBlock.filesDescriptors[index] = mft_rec;
  
  printf("---------- Novo registro no MFT ----------\n");
  printf("| Nome: %s\n", mftBlock.filesDescriptors[index]->record->name);
  printf("| MFTNumber: %i\n", mftBlock.filesDescriptors[index]->record->MFTNumber);
  printf("| Type: regular\n");
  printf("------------------------------------------\n\n");
  
  return 0;
  
  //
  // FALTA RETORNAR O FILE
  //
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
  
  char dir[MAX_FILE_NAME_SIZE], subdir[MAX_FILE_NAME_SIZE], subdir_copy[MAX_FILE_NAME_SIZE], *token;
  const char s[2] = "/";
  FILA2 entradas;
  
  CreateFila2(&entradas);
  strcpy(dir, "/");
  
  // pega o primeiro subdir de "/"
  strcpy(subdir, strtok(pathname, s));
  
  int can_make_dir = 0;
  
  // percorre todos subdiretórios do path
  while(subdir != NULL && !can_make_dir) {
    // coloca em na fila entradas os registros do dir atual
    if (readEntradas2(mftBlock.root, &entradas) != 0) {
      printf("[ERRO] Erro ao tentar criar %s\n\n", pathname);
      return -1;
    }
    
    // se erro ao ler registro das entradas, já retorna -1
    if (FirstFila2(&entradas) != 0) {
      printf("[ERRO] Erro ao ler registro nas entradas\n\n");
      return -1;
    }
    // pega o primeiro registro
    struct t2fs_record *record;
    record = GetAtIteratorFila2(&entradas);
    
    int found_subdir = 0;
    int should_tokenize = 1;
    
    // se diretório nao tem entradas ainda
    if (record == NULL) {
      strcpy(subdir_copy, subdir);
      
      strcpy(subdir, strtok(NULL, s));
      
      // se for o último subdiretório do path e pai ainda não tem entradas
      // quer dizer que pode criar diretorio
      if (subdir == NULL) {
        found_subdir = 1;
        can_make_dir = 1;
        
      // mas se nao tem entradas e deveria ter um dir intermediario => erro
      } else {
        printf("[ERRO] Não existem diretórios intermediários\n\n");
        return -1;
      }
    }
    
    // percorre todas entradas do diretorio
    while (record != NULL && !found_subdir) {
      strcpy(subdir_copy, subdir);
      
      strcpy(subdir, strtok(NULL, s));
      should_tokenize = 0;
      
      // se encontrou o registro de mesmo nome do subdir que está conferindo
      if (strcmp(record->name, subdir) == 0) {
        // se subdir é o último dir do pathname solicitado,
        // significa que já existe um record com este path absoluto
        if (subdir == NULL) {
          printf("[ERRO] Já existe algo com esse nome\n\n");
          return -1;
          
        // se não for, quer dizer que até agora o pai existe e vai continuar buscando no laço de fora
        } else {
          found_subdir = 1;
          strcpy(dir, subdir_copy);
        }
        
      // se não encontrou o registro com nome igual ao subdir que está conferindo
      } else {
        // se este é o path final que quero criar, marca flag dizendo que estrutura de subdiretórios está ok e não existe nada com este path absoluto
        if (subdir == NULL) {
          can_make_dir = 1;
        }
      }
      NextFila2(&entradas);
      record = GetAtIteratorFila2(&entradas);
    }
    
    // se percorreu todas entradas e não achou subdiretório lá, deu erro
    if (!found_subdir) {
      printf("[ERRO] Não existe diretório pai, avô ou outro\n\n");
      return -1;
    }
    
    printf("%s\n", subdir);
    if (should_tokenize) {
      strcpy(subdir, strtok(NULL, s));
    }
  }
  
  // se percorreu todo o path e não marcou flag que pode criar, deu erro
  if (!can_make_dir) {
    printf("[ERRO] Erro ao criar diretório\n\n");
    return -1;
  }
  
  printf("Finalmente vou criar o dir: %s. Seu pai é: %s\n", subdir_copy, dir);
  
  // AQUI DEVE CRIAR ENTRADA NO DIRETORIO PAI

  return(0);
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
  char c;
  int i = 0;
  
  // garante que começa no root
  c = pathname[0];
  if (c != '/') {
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
