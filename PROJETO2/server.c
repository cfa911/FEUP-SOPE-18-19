#include <stdio.h>
#include "server.h"
#include "constants.h"
#include "types.h"
#include "thread_function.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include <time.h>

bank_account_t accounts[MAX_BANK_ACCOUNTS];

int main(int argc, char *argv[])
{
  srand(time(NULL)); //Randomize time
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
  bank_account_t admin_account;
  admin_account.account_id = ADMIN_ACCOUNT_ID;
  admin_account.balance = 0;


  //GENERATE SALT
  int MAX_NUMBER = pow(2,30);
  int salt_number = rand() % MAX_NUMBER + 1;


  printf("%i\n",salt_number);

  //sha256
  //echo -n "password""salt" | sha256sum
  char result[WIDTH_ID];
  char salt[SALT_LEN+1];

  char code[30] = "echo -n ";
  char output[HASH_LEN+1];
  sprintf(result, "%i", ADMIN_ACCOUNT_ID); //future iterations change ADMIN_ACCOUNT_ID to any ID
  sprintf(salt, "%i", salt_number);


  strcat(code, argv[2]); // password
  strcat(code, " ");
  strcat(code, salt);//salt
  strcat(code, " | sha256sum");
  commandHash = popen(code, "r");
  fgets(output, HASH_LEN + 1, commandHash); //read 64 bytes
  strcpy(admin_account.hash, output); //output is hashed password
  strcpy(admin_account.salt, salt); //the Salt

  printf("%s\n",output);


  //struct bank_account *accounts = malloc(atoi(argv[1]) * sizeof(struct bank_account)); // creates memory for accounts

  accounts[ADMIN_ACCOUNT_ID] = admin_account; //SET ADMIN ACCOUNT TO POSX 0

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
  tlv_request_t structure;
  pthread_t threads[MAX_BANK_OFFICES];
  for (size_t i = 0; i < atoi(argv[1]); i++) // creates threads/balcoes eletronicos
  {
    pthread_t tid;
    if (pthread_create(&tid, NULL, thread_function, &i) != 0)
    {
      printf("Error creating bank offices");
      exit(1);
    }
    threads[i] = tid;
  }

  for (size_t i = 0; i < atoi(argv[1]); i++) // Waits for threads/balcoes eletronicos
  {
    if (pthread_join(threads[i], NULL) != 0)
    {
      printf("Error waiting for bank offices");
      exit(1);
    }
  }
  char a[2];
  scanf("%s", a);

  while (strcmp(a, "q") != 0)
  {
    if(read(fd, &structure, sizeof(structure)) > 0)
    {
    printf("\nPID:%i\n", structure.value.header.pid);
    printf("DELAY:%i\n", structure.value.header.op_delay_ms);
    printf("account_id:%i\n", structure.value.header.account_id);

    }
    getchar();
    scanf("%s", a);
    /* code */
  }
  

  close(fd);
  remove(SERVER_FIFO_PATH);
  return 0;
}

// void validateAccount(tlv_request_t request){
//
//   req_value_t reqvalue;
//   request.value = reqvalue.header;
//   req_header_t headervalidate;
//
//   if(headervalidate.pid != || headervalidate.account_id != || headervalidate.password != || headervalidate.op_delay_ms != ){
//
//
//
//   }
//
// }

void print_usage(FILE *stream, char *progname)
{
  fprintf(stream, "usage: %s <nr_bank_offices_to_create> <password>\n", progname);
}
