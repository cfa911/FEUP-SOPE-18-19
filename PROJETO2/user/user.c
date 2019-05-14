#include <stdio.h>
#include "user.h"
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


int main(int argc, char* argv[]) {

  if (argc != 6) {
          printf("Wrong number of arguments\n");
          print_usage(stderr, argv[0]);
          return -1;
      }


}


void print_usage(FILE * stream, char * progname) {
    fprintf(stream, "usage: %s <account_id> <password> <delay> <code> <list>\n", progname);
}
