#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <apidisk.h>
#include <bitmap2.h>
#include <t2fs.h>
#include <mft_manager.h>
#include <implementation.h>

struct t2fs_4tupla createTupla(DWORD atributeType, DWORD vbn, DWORD lbn, DWORD contiguosBlocks) {
  struct t2fs_4tupla tupla;
  
  tupla.atributeType = atributeType;
  tupla.virtualBlockNumber = vbn;
  tupla.logicalBlockNumber = lbn;
  tupla.numberOfContiguosBlocks = contiguosBlocks;
  
  return tupla;
}

void print_the_sound_of_a_capybara() {
  printf("\nAheoUYUOoomTgUU\n");
  
  // unsigned char buffer[SECTOR_SIZE];
  // char *p1 = strtok(NULL, " ");
  // if (p1==NULL) {
  //   printf ("Erro no comando.\n");
  // }
  // int sector = atoi(p1);
  // int error = read_sector (sector, buffer);
}