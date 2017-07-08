#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <t2fs.h>
#include <support.h>
#include <implementation.h>


int main() {
  printf("\nIniciando Teste\n\n");

  mkdir2("/goiaba");
  create2("/goiaba/test");
  
  create2("/arroz");
  mkdir2("/arroz/feijao");
  
  create2("/arroz/feijao/bla");
    
  return 0;
}
