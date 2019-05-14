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

static FILE * ulogFile = NULL;
static int timeout = 0;

int main(int argc, char* argv[]) {

  if (argc != 6) {
          printf("Wrong number of arguments\n");
          print_usage(stderr, argv[0]);
          return -1;
      }

  int pid = getpid();
  printf("** Running process %d **  \n", pid);

  int secure_srv = open("secure_srv", O_RDWR);

  if(secure_srv == -1){
   printf("Error opening secure_srv FIFO\n");
   return -3;
  }

  char sn[20];
  //sprintf(sn, "secure_%d", pid);
  printf("abort?");

  if(mkfifo(sn, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) != 0){
    fprintf(stderr, "Error creating secure_srv fifo\n");
    return -1;
  }

  int status = open(sn, O_RDWR | O_NONBLOCK );

  if(status == -1){
   printf("Error opening status FIFO\n");
   return -2;
 }

 char msg[250] = {0};

  while (!timeout)
  {
      read(status, msg, sizeof(msg));
      if (strlen(msg) > 0)
      {
        break;
      }

      sleep(1);
  }
  if(strlen(msg) > 0)
    write_to_ulog(msg);

  close(status);
  close(secure_srv);

  remove(sn);

  return 0;

}


void print_usage(FILE * stream, char * progname) {
    fprintf(stream, "usage: %s <account_id> <password> <delay> <code> <list>\n", progname);
}

int write_to_ulog(char* status){

  ulogFile = fopen("ulog.txt", "a");
    int values[250];
    int t = 0;
    int i;
  char* token = strtok(status, " ");

while (token) {
  values[t] = atoi(token);
  t++;
  token = strtok(NULL, " ");
}

if (values[0] > 0){
  for(i = 1; i<= values[0]; i++){
    fprintf(ulogFile, getpid(), i, values[0], values[i]);
    fflush(ulogFile);
  }
}


}
