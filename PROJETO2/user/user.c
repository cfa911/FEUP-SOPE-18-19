#include <stdio.h>
#include "user.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "../constants.h"

//codigos das operacoes:
// 0 - criacao de contas
// 1 - consulta de saldo
// 2 - relaziacao de transferencias
// 3 - encerramento do servidor

int main(int argc, char *argv[])
{
  char pid_string[6];
  char fifo_user[20] = "secure_";

  if (argc != 6)
  {
    printf("Wrong number of arguments\n");
    print_usage(stderr, argv[0]);
    return -1;
  }

  int pid = getpid();
  printf("Pid del processio %i\n", pid);
  int fifo_server = open(SERVER_FIFO_PATH, O_WRONLY | O_NONBLOCK);
  if(fifo_server < 0)
  {
    perror("Error: Failed to open fifo\n");
    exit(0);
  }
  sprintf(pid_string, "%i", pid); //future iterations change ADMIN_ACCOUNT_ID to any ID
  strcat(fifo_user, pid_string);
  if (mkfifo(fifo_user, 0660) != 0)
  {
    perror("Error: Failed to open fifo\n");
    exit(0);
  }

  int fd = open(fifo_user, O_WRONLY | O_NONBLOCK);


  remove(fifo_user);
  close(fd);
  close(fifo_server);

}

void print_usage(FILE *stream, char *progname)
{
  fprintf(stream, "usage: %s <account_id> <password> <delay> <code> <list>\n", progname);
}
