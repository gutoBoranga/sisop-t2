#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <t2fs.h>
#include <support.h>
#include <implementation.h>

FILA2 dirList;

int main() {    //COMENTA O TESTE QUE TU NAO QUISER TESTAR
                //OU NAO, PODE TENTAR EXECUTAR OS DOIS JUNTOS MAS NAO SEI SE DA CERTO
                
   //TESTE DO BUSCA DIRETORIO PAI
  char *pathname = "/a";

    CreateFila2(&dirList);
    registro_dir *t2fs_reg;

    DIRETORIO *dir1 = malloc(sizeof(DIRETORIO));
    t2fs_reg = (registro_dir *) malloc(sizeof(registro_dir));
    strcpy(t2fs_reg->name, "/");
    dir1->t2fs_reg = t2fs_reg;
    dir1->pai_pathname = "/";
    dir1->handle = 1;
    AppendFila2(&dirList, dir1);

    DIRETORIO *dir2 = malloc(sizeof(DIRETORIO));
    t2fs_reg = (registro_dir *) malloc(sizeof(registro_dir));
    strcpy(t2fs_reg->name, "diretorio2");
    dir2->t2fs_reg = t2fs_reg;
    dir2->pai_pathname = "/";
    dir1->handle = 2;
    AppendFila2(&dirList, dir2);

    DIRETORIO *dir3 = malloc(sizeof(DIRETORIO));
    t2fs_reg = (registro_dir *) malloc(sizeof(registro_dir));
    strcpy(t2fs_reg->name, "diretorio3");
    dir3->t2fs_reg = t2fs_reg;
    dir3->pai_pathname = "/diretorio2";
    dir1->handle = 3;
    AppendFila2(&dirList, dir3);


    DIRETORIO *paizao;
    paizao = buscaDiretorioPai(pathname, strlen(pathname));
    if(paizao != NULL){
    	printf("\nO nome do paizao é %s", paizao->t2fs_reg->name);
    	printf("\nNome do pai do diretorio3 : %s", dir3->pai_pathname);
    }

    //TESTE DO READ ENTRADAS
    /* struct t2fs_record registroTeste;

    registroTeste.TypeVal = 2;
    strcpy(registroTeste.name, "testezera");
    registroTeste.blocksFileSize = 1;
    registroTeste.bytesFileSize = 1;
    registroTeste.MFTNumber = 1;

    char buffer[64];
    memcpy(buffer, &registroTeste, sizeof(registroTeste));
    strcpy(buffer, (char*)registroTeste.TypeVal);
    strcat(buffer, (char*)registroTeste.name);
    strcat(buffer, (char*)registroTeste.blocksFileSize);
    strcat(buffer, (char*)registroTeste.bytesFileSize);
    strcat(buffer, (char*)registroTeste.MFTNumber);
    printf("\n buffer = %s", buffer);

    readEntradas(buffer);*/



    return 0;
}
