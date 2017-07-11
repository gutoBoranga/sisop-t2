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
void init_implementation();
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

	//pega nome do novo arquivo e path do pai
	char *arqName;

	char *paiPath;
	paiPath = pathDoPai(filename);
	novo_arquivo->pai_pathname = paiPath;

	//cria um novo t2fs_record
	registro_dir *t2fs_reg = (registro_dir *) malloc(sizeof(registro_dir));
	t2fs_reg->TypeVal = 1;
	arqName = nameFromPath(filename);
	memcpy(t2fs_reg->name, arqName, MAX_FILE_NAME_SIZE);
	t2fs_reg->blocksFileSize = 0;
	t2fs_reg->bytesFileSize = 0;
	t2fs_reg->MFTNumber = busca_regMFT_livre();

	//salva novo t2fs_record no diretorio pai e no novo arquivo
	AppendFila2(&dirPai->entradas, t2fs_reg);
	novo_arquivo->t2fs_reg = t2fs_reg;

	//inicializa dados de execução do novo arquivo
	novo_arquivo->current_pointer = 0;

	last_arq_handle++;
	novo_arquivo->handle = last_arq_handle;

	//coloca arquivo na lista de arquivos abertos
	AppendFila2(&arqList, novo_arquivo);

	return novo_arquivo->handle;
}


int delete2 (char *filename) {
  return -1;
}


FILE2 open2 (char *filename) {
	if (!has_initialized) {
		init_blocks();
		has_initialized = 1;
	}

	if (!path_is_valid(filename, TYPEVAL_REGULAR)) {
		printf("[ERRO] Path %s inválido\n\n", filename);
		return -1;
	}

	if(arquivo_ja_aberto(filename) != 0){
		return -1;
	}

	DIRETORIO *dirPai;
	dirPai = buscaDiretorioPai(filename, strlen(filename));
	if(dirPai == NULL)
		return -1;

	ARQUIVO *novo_arquivo;
	novo_arquivo = (ARQUIVO *) malloc(sizeof(ARQUIVO));

	//inicializa caminho do diretório pai
	char *paiPath;
	paiPath = pathDoPai(filename);
	novo_arquivo->pai_pathname = paiPath;

	//pega t2fs_reg do diretório pai e salva no novo arquivo
	registro_dir *t2fs_reg = get_t2fs_record_from_dir(dirPai, nameFromPath(filename));
	novo_arquivo->t2fs_reg = t2fs_reg;

	//inicializa dados de execução do novo arquivo
	novo_arquivo->current_pointer = 0;

	last_arq_handle++;
	novo_arquivo->handle = last_arq_handle;

	//coloca arquivo na lista de arquivos abertos
	AppendFila2(&arqList, novo_arquivo);

	return novo_arquivo->handle;
}


int close2 (FILE2 handle) {
  return -1;
}


int read2 (FILE2 handle, char *buffer, int size) {
  	if (!has_initialized) {
    init_blocks();
    has_initialized = 1;
  }

	ARQUIVO *arq;
	arq = getArquivo(handle);

	unsigned char blockBuffer[BLOCK_SIZE];

	/*while(){
		if(readBlock(blocoAtualNumber,blockBuffer)  != 0)
			return -1;

	}*/
    return -1;
}


int write2 (FILE2 handle, char *buffer, int size) {
  return -1;
}


int truncate2 (FILE2 handle) {
  return -1;
}


int seek2 (FILE2 handle, DWORD offset) {
  return -1;
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

	DIRETORIO *dirPai;
	dirPai = buscaDiretorioPai(pathname, strlen(pathname));
	if(dirPai == NULL)
		return -1;

	DIRETORIO *novo_diretorio;
	novo_diretorio = (DIRETORIO *) malloc(sizeof(DIRETORIO));

	//pega nome do novo arquivo e path do pai
	char *dirName;

	char *paiPath;
	paiPath = pathDoPai(pathname);
	novo_diretorio->pai_pathname = paiPath;

	//cria um novo t2fs_record
	registro_dir *t2fs_reg = (registro_dir *) malloc(sizeof(registro_dir));
	t2fs_reg->TypeVal = TYPEVAL_DIRETORIO;
	dirName = nameFromPath(pathname);
	memcpy(t2fs_reg->name, dirName, MAX_FILE_NAME_SIZE);
	t2fs_reg->blocksFileSize = 0;
	t2fs_reg->bytesFileSize = 0;
	t2fs_reg->MFTNumber = busca_regMFT_livre();

	//salva novo t2fs_record no diretorio pai e no novo arquivo
	AppendFila2(&dirPai->entradas, t2fs_reg);
	novo_diretorio->t2fs_reg = t2fs_reg;

	//inicializa dados de execução do novo arquivo
	novo_diretorio->current_entry = 0;

	last_dir_handle++;
	novo_diretorio->handle = last_dir_handle;

	//coloca arquivo na lista de arquivos abertos
	AppendFila2(&dirList, novo_diretorio);

	return novo_diretorio->handle;
}


int rmdir2 (char *pathname) {
  return -1;
//   if (!path_is_valid(pathname, TYPEVAL_DIRETORIO)) {
//     printf("[ERRO] Path %s inválido\n\n", pathname);
//     return -1;
//   }
//
//   int current_max = mftBlock.next_valid_MFTnumber - NUM_SPECIAL_RECORDS;
//
//   // se não existe nenhum registro
//   if (current_max <= 0) {
//     return -1;
//   }
//
//   char descriptor[MAX_FILE_NAME_SIZE];
//   int i, number_of_matches;
//   i = number_of_matches = 0;
//
//   mft_record *mft_rec;
//
//   while(i < current_max) {
//     strncpy(descriptor, mftBlock.filesDescriptors[i]->record->name, strlen(pathname));
//
//     // vê se encontrou o diretório em questão
//     if (pathsAreEquivalent(pathname, mftBlock.filesDescriptors[i]->record->name)) {
//       if (mftBlock.filesDescriptors[i]->record->TypeVal != TYPEVAL_DIRETORIO) {
//         printf("[ERRO] Path %s não é um diretório\n\n", pathname);
//         return -1;
//       // encontrou o registro E É UM DIRETORIO UHUUL
//       } else {
//         mft_rec = mftBlock.filesDescriptors[i];
//         number_of_matches += 1;
//       }
//
//     // vê se encontra algum subdiretório
//     } else if (pathsAreEquivalent(pathname, descriptor)) {
//       number_of_matches += 1;
//     }
//     i++;
//   }
//
//   if (number_of_matches == 0) {
//     printf("[ERRO] Path %s não corresponde a um diretório\n\n", pathname);
//     return -1;
//   } else if (number_of_matches > 1) {
//     printf("[ERRO] Diretório %s não pode ser removido pois não está vazio\n\n", pathname);
//     return -1;
//   }
//
//   // seta flag do registro como inválido
//   mft_rec->valid = MFT_RECORD_INVALID;
//
//   // cria nodo pra lista de indices removidos
//   di_node *node = createDInode(mft_rec->record->MFTNumber);
//   mftBlock.deleted_indexes = appendDInode(node, mftBlock.deleted_indexes);
//
//   return 0;
}


DIR2 opendir2 (char *pathname) {
  if (!has_initialized) {
		init_blocks();
		has_initialized = 1;
	}

	if (!path_is_valid(pathname, TYPEVAL_DIRETORIO)) {
		printf("[ERRO] Path %s inválido\n\n", pathname);
		return -1;
	}

	if(diretorio_ja_aberto(pathname) != 0){
		return -1;
	}

	DIRETORIO *dirPai;
	dirPai = buscaDiretorioPai(pathname, strlen(pathname));
	if(dirPai == NULL)
		return -1;

	DIRETORIO *novo_diretorio;
	novo_diretorio = (DIRETORIO *) malloc(sizeof(DIRETORIO));

	//inicializa caminho do diretório pai
	char *paiPath;
	paiPath = pathDoPai(pathname);
	novo_diretorio->pai_pathname = paiPath;

	//pega t2fs_reg do diretório pai e salva no novo arquivo
	registro_dir *t2fs_reg = get_t2fs_record_from_dir(dirPai, nameFromPath(pathname));
	novo_diretorio->t2fs_reg = t2fs_reg;

	//inicializa dados de execução do novo arquivo
	novo_diretorio->current_entry = 0;

	last_dir_handle++;
	novo_diretorio->handle = last_dir_handle;

	//coloca arquivo na lista de arquivos abertos
	AppendFila2(&dirList, novo_diretorio);

	return novo_diretorio->handle;
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
  init_implementation();
  initMFTBlock();
}

void initBootBlock() {
  strcpy(bootBlock.id, "T2fs");
  bootBlock.version = 0x7E11;
  bootBlock.blockSize = 0x0004;
  bootBlock.MFTBlocksSize = 0x0800;
  bootBlock.diskSectorSize = 0x8000;
}

void init_implementation(){
	CreateFila2(&dirList);
	CreateFila2(&arqList);

	last_dir_handle = 0;
	last_arq_handle = 0;

	//inicializa o diretorio root
	DIRETORIO *root;
	root = (DIRETORIO *) malloc(sizeof(DIRETORIO));

	//cria um novo t2fs_record para o root
	registro_dir *t2fs_reg = (registro_dir *) malloc(sizeof(registro_dir));
	t2fs_reg->TypeVal = 2;
	strcpy(t2fs_reg->name, "/");
	t2fs_reg->blocksFileSize = 0;
	t2fs_reg->bytesFileSize = 0;
	t2fs_reg->MFTNumber = 2;

	//inicializa dados de execução do novo arquivo
	root->t2fs_reg = t2fs_reg;
	root->current_entry = 0;
	root->handle = 0;
	root->pai_pathname = "/";

	//le as entradas do root

	//coloca root na lista de diretorios abertos
	AppendFila2(&dirList, root);
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
