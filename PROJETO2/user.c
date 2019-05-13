#include <stdio.h>
#include "user.h"
#include "server.h"
#include "constants.h"
#include "types.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

//codigos das operacoes:
// 0 - criacao de contas
// 1 - consulta de saldo
// 2 - relaziacao de transferencias
// 3 - encerramento do servidor


int main(int argc, char * argv[]) {

  if (argc != 6) {
          printf("Wrong number of arguments\n");
          print_usage(stderr, argv[0]);
          return -1;
      }

  switch(argv[4]){
    case 0:
    //createAccount();
    break;
    case 1:
    //funcao para consulta de saldo
    break;
    case 2:
    //funcao realizacao de transferencias
    break;
    case 3:
    //funcao para encerramento do servidor
    break;
  }
}


void print_usage(FILE * stream, char * progname) {
    fprintf(stream, "usage: %s <account_id> <password> <delay> <code> <list>\n", progname);
}
