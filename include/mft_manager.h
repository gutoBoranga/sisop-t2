#include <t2fs.h>

#define MFT_BLOCK_SIZE 0x0800

// === Structs ===============

struct t2fs_mftBlock {		// Tabela 1 � Descri��o dos campos do bloco de boot
  int length;                           // número de registros válidos
	struct t2fs_record bitmapDescriptor;  // registro 0: descritor do arquivo que fornece o bitmap de blocos de dados livres e ocupados;
  struct t2fs_record rootDescriptor;    // registro 1: descritor do arquivo associado ao diretório raiz;
  struct t2fs_record record2;           // registro 2: reservado para uso futuro
  struct t2fs_record record3;           // registro 3: reservado para uso futuro
  struct t2fs_record filesDescriptors[MFT_BLOCK_SIZE - 4]; // outros registros
};

// === Function declarations ===============

void print_the_sound_of_a_capybara();