#include <stdio.h>
#include "server.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>


int main(int argc, char * argv[]) {

  if (argc != 3) {
          printf("Wrong number of arguments\n");
          print_usage(stderr, argv[0]);
          return -1;
      }

  // CRIAR CONTA ADMINISTRADOR

  //criar FIFO secure_srv
  if(mkfifo("tmp/secure_srv", 0660) != 0) {
        fprintf(stderr, "Error creating secure_srv fifo\n");
        return -2;
  }

  int nr_bank_offices_to_create = atoi(argv[1]);
  //char password[] = atoi(argv[2]);

  listen_for_securesrv();

}

int listen_for_securesrv() {
    int fifo_read_fd = open("tmp/secure_srv", O_RDONLY);

    if(fifo_read_fd == -1) {
        return -1;
    }

    return 0;
}

void print_usage(FILE * stream, char * progname) {
    fprintf(stream, "usage: %s <nr_bank_offices_to_create> <password>\n", progname);
}
