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
  char *pathname = "/diretorio2/diretorio3/";

    CreateFila2(&dirList);

    DIRETORIO *dir1 = malloc(sizeof(DIRETORIO));
    dir1->name = "diretorio1";
    dir1->pai_pathname = "/";
    AppendFila2(&dirList, dir1);

    DIRETORIO *dir2 = malloc(sizeof(DIRETORIO));
    dir2->name = "diretorio2";
    dir2->pai_pathname = "/";
    AppendFila2(&dirList, dir2);

    DIRETORIO *dir3 = malloc(sizeof(DIRETORIO));
    dir3->name = "diretorio3";
    dir3->pai_pathname = "/diretorio2";
    AppendFila2(&dirList, dir3);


    DIRETORIO *paizao;
    paizao = buscaDiretorioPai(pathname, strlen(pathname));
    if(paizao != NULL){
    	printf("\nO nome do paizao é %s", paizao->name);
    	printf("\nNome do pai do diretorio3 : %s", dir3->pai_pathname);
    }

    //TESTE DO READ ENTRADAS
     struct t2fs_record registroTeste;

    registroTeste.TypeVal = 2;
    strcpy(registroTeste.name, "testezera");
    registroTeste.blocksFileSize = 1;
    registroTeste.bytesFileSize = 1;
    registroTeste.MFTNumber = 1;

    char buffer[64];
    memcpy(buffer, &registroTeste, sizeof(registroTeste));
    /*strcpy(buffer, (char*)registroTeste.TypeVal);
    strcat(buffer, (char*)registroTeste.name);
    strcat(buffer, (char*)registroTeste.blocksFileSize);
    strcat(buffer, (char*)registroTeste.bytesFileSize);
    strcat(buffer, (char*)registroTeste.MFTNumber);*/
    printf("\n buffer = %s", buffer);

    // readEntradas(buffer);



    return 0;
}
