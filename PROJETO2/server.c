#include <stdio.h>
#include "server.h"
#include "constants.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>


int main(int argc, char * argv[]) {

  if (argc != 3) {
          printf("Wrong number of arguments\n");
          print_usage(stderr, argv[0]);
          return -1;
      }

  if(*argv[1] > MAX_BANK_OFFICES){
    printf("Number of bank offices is too high\n");
    return -2;
  }

  // CRIAR CONTA ADMINISTRADOR
  //BLA
  //BLA
  //BLA

  if(mkfifo("secure_srv", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) != 0) {
        fprintf(stderr, "Error creating secure_srv fifo\n");
        return -3;
  }

  int fd = open("secure_srv", O_RDONLY | O_NONBLOCK);

  if(fd == -1) {
     printf("Error opening FIFO\n");
     return -1;
  }

    close("fd");
    remove("secure_srv");

     return 0;
}


void print_usage(FILE * stream, char * progname) {
    fprintf(stream, "usage: %s <nr_bank_offices_to_create> <password>\n", progname);
}
