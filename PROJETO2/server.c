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
#include "thread_function.h"

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
  else if (strlen(argv[2]) < MIN_PASSWORD_LEN)
  {
    printf("Password too short\n");
    return -3;
  }
  else if (strlen(argv[2]) > MAX_PASSWORD_LEN)
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
  char result[WIDTH_ID];
  char code[30] = "echo -n ";
  char output[HASH_LEN];
  sprintf(result, "%i", ADMIN_ACCOUNT_ID); //future iterations change ADMIN_ACCOUNT_ID to any ID

  strcat(code, argv[2]);
  strcat(code, " | sha256sum");

  //printf("%s\n",code);  //prints code

  commandHash = popen(code, "r");
  fgets(output, HASH_LEN + 1, commandHash); //read 64 bytes
  //printf("%s\n", output); //prints hash

  strcpy(admin_account.password, output); //output is hashed password

  struct req_create_account *accounts = malloc(atoi(argv[1]) * sizeof(struct req_create_account)); // creates memory for accounts

  accounts = &admin_account;

  //criar fifo secure_srv
  if (mkfifo(SERVER_FIFO_PATH, 0660) != 0)
  {
    fprintf(stderr, "Error creating secure_srv fifo\n");
    return -3;
  }

  int fd = open(SERVER_FIFO_PATH, O_RDONLY | O_NONBLOCK);

  if (fd == -1)
  {
    printf("Error opening FIFO\n");
    return -1;
  }
  getchar();

  pthread_t threads[MAX_BANK_OFFICES];
  for (size_t i = 0; i < atoi(argv[1]); i++) // creates threads/balcoes eletronicos
  {
    pthread_t tid;
    if (pthread_create(&tid, NULL, thread_function, &i) != 0)
    {
      printf("Error creating electronic offices");
      exit(1);
    }
    threads[i] = tid;
    /* code */
  }

 for (size_t i = 0; i < atoi(argv[1]); i++) // Waits for threads/balcoes eletronicos
  {
    pthread_t tid;
    if (pthread_join(threads[i],NULL) != 0)
    {
      printf("Error waiting for electronic offices");
      exit(1);
    }
  }
  
  close(fd);
  remove(SERVER_FIFO_PATH);

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
