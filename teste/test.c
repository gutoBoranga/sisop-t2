#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <t2fs.h>
#include <support.h>
#include <implementation.h>

int main() {
  char *bla;
  bla = "oi";
  bla = pathDoPai("/a/b/c");
  printf("%s\n", bla);
  
  // printf("%s\n", nameFromPath("bla/capivara/verde"));
  // mkdir2("/a/");
  return 0;
}
