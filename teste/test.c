#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <t2fs.h>
#include <support.h>
#include <implementation.h>

void a() {
  mkdir2("/goiaba");
  mkdir2("/batata");
  mkdir2("/arroz");
  mkdir2("/pastel");
  open2("/");
  
  rmdir2("/arroz");
  printf("\nRemoveu\n\n");
  
  open2("/");
  //
  mkdir2("/catioro");
  
  open2("/");
}

int main() {
  mkdir2("/a/");
  mkdir2("/a/b");
  mkdir2("/a/b/c");
  mkdir2("/goiaba");
  mkdir2("/batata");
  mkdir2("/arroz");
  create2("/batata/frita");
  //
  rmdir2("/a/");
  rmdir2("2rh38r9239");
  rmdir2("/batata/frita");
  rmdir2("/goiaba");
  rmdir2("/arroz");
  
  create2("/batata/assada");
  create2("/algo");
  mkdir2("/meu-compiuter");
  
  delete2("i23h2");
  delete2("/algo");
  delete2("/a/b/c/");
  
  create2("/meu-compiuter/um-arquivo");
  // open2 printa a lista de registros na mft por enquanto
  open2("/");
    
  return 0;
}
