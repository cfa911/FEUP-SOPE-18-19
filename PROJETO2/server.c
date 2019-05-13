#include <stdio.h>
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

int main(int argc, char *argv[])
{
  FILE *commandHash;
  // CHAIN OF REQUIREMENTS
  if (argc != 3)
  {
    printf("Wrong number of arguments\n");
    print_usage(stderr, argv[0]);
    return -1;
  }
  else if (*argv[1] > MAX_BANK_OFFICES)
  {
    printf("Number of bank offices is too high\n");
    return -2;
  }
  else if (strlen(argv[1]) < 8)
  {
    printf("Password too short\n");
    return -3;
  }
  else if (strlen(argv[1]) > 20)
  {
    printf("Password too long\n");
    return -3;
  }

  // CRIAR CONTA ADMINISTRADOR
  req_create_account_t admin_account;
  admin_account.account_id = ADMIN_ACCOUNT_ID;
  admin_account.balance = 0;

  //sha256
  //echo password | sha256sum > sumfile
  char filename[10];
  char result[3];
  char code[30] = "echo -n ";
  char output[64];
  sprintf(result, "%i", ADMIN_ACCOUNT_ID); //future iterations change ADMIN_ACCOUNT_ID to any ID

  strcat(code, argv[1]);
  strcat(code, " | sha256sum");

  //printf("%s\n",code);  //prints code

  commandHash = popen(code, "r");
  fgets(output, 65, commandHash); //read 64 bytes
  //printf("%s\n", output); //prints hash

  

  strcpy(admin_account.password, argv[2]);

  struct req_create_account *accounts = malloc(atoi(argv[1]) * sizeof(struct req_create_account));

  accounts = &admin_account;

  //criar fifo secure_srv
  if (mkfifo("secure_srv", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) != 0)
  {
    fprintf(stderr, "Error creating secure_srv fifo\n");
    return -3;
  }

  int fd = open("secure_srv", O_RDONLY | O_NONBLOCK);

  if (fd == -1)
  {
    printf("Error opening FIFO\n");
    return -1;
  }

  close(fd);
  remove("secure_srv");

  return 0;
}

void print_usage(FILE *stream, char *progname)
{
  fprintf(stream, "usage: %s <nr_bank_offices_to_create> <password>\n", progname);
}

void createAccount()
{

  int id, balance;
  char password[20];

  printf("Insert your id account: ");
  id = scanf("%d", &id);

  // VER SE ID JÁ ESTÁ A SER USADO POR OUTRA CONTA !!!!!!!!!!!

  if (id >= 1 || id <= MAX_BANK_ACCOUNTS)
  {

    printf("Insert your initial balance for the account: ");
    balance = scanf("%d", &balance);

    if (balance >= MIN_BALANCE || balance <= MAX_BALANCE)
    {

      printf("Insert your password account: ");
      scanf("%s", password);
    }
  }
}
