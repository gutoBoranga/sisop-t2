#include <t2fs.h>

#define MFT_BLOCK_SIZE 0x0800
#define NUM_TUPLAS 32
#define NUM_SPECIAL_RECORDS 4

#define MFT_RECORD_VALID 1
#define MFT_RECORD_INVALID 0

// === Structs ===============

typedef struct t2fs_mft_record {
  int valid;
  struct t2fs_record *record;
  struct t2fs_4tupla tuplas[NUM_TUPLAS];
} mft_record;

typedef struct deleted_index_node {
  int index;
  struct deleted_index_node *next;
} di_node;

struct t2fs_mftBlock {		// Tabela 1 � Descri��o dos campos do bloco de boot
  int next_valid_MFTnumber;             // número de registros válidos
	
  mft_record bitmapDescriptor;  // registro 0: descritor do arquivo que fornece o bitmap de blocos de dados livres e ocupados;
  mft_record rootDescriptor;    // registro 1: descritor do arquivo associado ao diretório raiz;
  mft_record descriptor2;           // registro 2: reservado para uso futuro
  mft_record descriptor3;           // registro 3: reservado para uso futuro
  mft_record **filesDescriptors; // outros registros
  
  di_node *deleted_indexes;
};

// === Function declarations ===============

di_node* createDInode(int i);
di_node* appendDInode(di_node *node, di_node *head);
di_node* removeFirstDInode(di_node *head);

struct t2fs_4tupla createTupla(DWORD atributeType, DWORD vbn, DWORD lbn, DWORD contiguosBlocks);
void print_the_sound_of_a_capybara();