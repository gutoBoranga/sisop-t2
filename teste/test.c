#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <t2fs.h>
#include <support.h>
#include <implementation.h>


int main() {
  // printf("\nIniciando Teste\n\n");
  //
  mkdir2("/a/");
  mkdir2("/a");
  mkdir2("/b");
  mkdir2("/b/");
  mkdir2("/c");
  mkdir2("/c");
  
  // rmdir2("/sda");
  
  // mkdir2("/a/b/c");
  // mkdir2("/aqui/");
  // mkdir2("/aqui/tem/");
  // mkdir2("/aqui/tem/coragem/");
  // mkdir2("/pao/com/chimia");
  
  return 0;
}
