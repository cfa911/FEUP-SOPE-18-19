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
#include "../types.h"

//codigos das operacoes:
// 0 - criacao de contas
// 1 - consulta de saldo
// 2 - relaziacao de transferencias
// 3 - encerramento do servidor

int main(int argc, char *argv[])
{
  char pid_string[6];
  char USER_FIFO_PATH[USER_FIFO_PATH_LEN] = "/tmp/secure_";

  if (argc != 6)
  {
    printf("Wrong number of arguments\n");
    print_usage(stderr, argv[0]);
    return -1;
  }

  int pid = getpid();
  printf("Process pid %i\n", pid);

  int fifo_server = open(SERVER_FIFO_PATH, O_WRONLY | O_NONBLOCK);
  //int fifo_server = open("file.txt", O_RDWR | O_CREAT, 777);


  if (fifo_server < 0)
  {
    perror("Error: Failed to open fifo\n");
    exit(0);
  }
  sprintf(pid_string, "%i", pid);
  strcat(USER_FIFO_PATH, pid_string);
  if (mkfifo(USER_FIFO_PATH, 0660) != 0)
  {
    perror("Error: Failed to open fifo\n");
    exit(0);
  }

  int fd = open(USER_FIFO_PATH, O_RDWR | O_NONBLOCK);

  struct tlv_request request;

  switch (atoi(argv[__OP_MAX_NUMBER]))
  {
  case OP_CREATE_ACCOUNT:
    request = create_account(argv[1], argv[2], argv[3], argv[5], pid);
    break;
  case OP_BALANCE:
    request = check_balance(argv[1], argv[2], argv[3], argv[5], pid);
    break;
  case OP_TRANSFER:
    request = make_transfer(argv[1], argv[2], argv[3], argv[5], pid);
    break;
  case OP_SHUTDOWN:
    request = shutdown_server(argv[1], argv[2], argv[3], argv[5], pid);
    break;
  default:
    printf("Invalid Option!\n");
    return 0;
    break;
  }
  printf("\n%i\n", request.value.header.pid);
  getchar();

  while (1)
  {
    printf("\nREQUEST: %i\n", request.value.header.pid);
    write(fifo_server, &request, request.length);
    

  }
  /*
  if (write(fifo_server, &request, request.length) == -1)
    printf("Invalid Option!\n");

  printf("\n\n%i\n",request.value.header.pid);

  struct tlv_request *updateData = malloc(sizeof(struct tlv_request));
  read(fifo_server, &updateData, sizeof(updateData));
  printf("\n\n%i\n",updateData->value.header.pid);
  //getchar();
  //read(fifo_server,&reply,66666);
  */

  /*
  THIS WORKS
  fwrite(&request, sizeof(struct tlv_reply), 1, outfile);
  if (fwrite != 0)
    printf("contents to file written successfully !\n");
  fclose(outfile);
  infile = fopen("person.dat", "r");

  while (fread(&input, sizeof(struct tlv_reply), 1, infile))
    printf("id = %d \n",input.value.header.account_id);
  fclose(infile);

  */
  remove(USER_FIFO_PATH);
  close(fd);
  close(fifo_server);
}

void print_usage(FILE *stream, char *progname)
{
  fprintf(stream, "usage: %s <account_id> <password> <delay> <code> <list>\n", progname);
  printf("\nCode of operacoes:\n");
  printf("0 - Account creation\n");
  printf("1 - Credit check\n");
  printf("2 - Make a transfer\n");
  printf("3 - Shutdown server\n\n");
}

tlv_request_t create_account(char *user, char *password, char *delay, char *args, int pid) //0
{
  int i = 0;
  char *tmp_str;

  if (strcmp(user, "0") != 0)
  {
    perror("Error: Not Admin!! Can't create accounts\n");
    exit(1);
  }

  req_create_account_t new_account;
  tmp_str = strtok(args, " "); //1st element

  new_account.account_id = atoi(tmp_str);
  tmp_str = strtok(NULL, " ");
  while (tmp_str != NULL)
  {

    if (i == 0)
    {
      new_account.balance = atoi(tmp_str);
      //2nd element
    }
    else if (i == 1)
    {
      strcat(new_account.password, tmp_str);
      //3rd element
    }
    else
    {
      perror("ERROR: TOO MANY ARGUMENTS ON ACCOUNT CREATION\n");
      exit(3);
    }
    tmp_str = strtok(NULL, " ");
    i++;
  }
  if (i != 2)
  {
    perror("ERROR: TOO LITTLE ARGUMENTS ON ACCOUNT CREATION\n");
    exit(4);
  }

  //check values

  if (new_account.balance < MIN_BALANCE || new_account.balance > MAX_BALANCE)
    exit(1);
  if (new_account.account_id <= ADMIN_ACCOUNT_ID || new_account.account_id > MAX_BANK_ACCOUNTS)
    exit(2);
  if (strlen(new_account.password) < MIN_PASSWORD_LEN || strlen(new_account.password) > MAX_PASSWORD_LEN)
    exit(3);

  //search and validade admin
  req_header_t user_info;
  user_info.account_id = atoi(user);
  strcat(user_info.password, password);
  user_info.pid = pid;
  user_info.op_delay_ms = atoi(delay);

  tlv_request_t request;
  request.type = OP_CREATE_ACCOUNT;
  request.length = sizeof user_info + sizeof new_account;
  request.value.header = user_info;
  request.value.create = new_account;

  return request;
}
tlv_request_t check_balance(char *user, char *password, char *delay, char *args, int pid)
{ //1
  req_header_t user_info;
  user_info.account_id = atoi(user);
  strcat(user_info.password, password);
  user_info.pid = pid;
  user_info.op_delay_ms = atoi(delay);

  tlv_request_t request;
  request.type = OP_BALANCE;
  request.length = sizeof user_info;
  request.value.header = user_info;
  // send request t
  return request;
}
tlv_request_t make_transfer(char *user1, char *password, char *delay, char *args, int pid)
{ //2
  //if validade user 1;

  char *tmp_str;
  int user2;
  int amount = 0;
  if (strcmp(args, "") == 0)
  {
    perror("ERROR: NO ARGUMENTS!!\n");
    exit(0);
  }
  tmp_str = strtok(args, " ");
  user2 = atoi(tmp_str);
  if (tmp_str != NULL)
  {
    tmp_str = strtok(NULL, " ");
    if (tmp_str == NULL)
    {
      perror("ERROR: ");
      exit(1);
    }
    amount = atoi(tmp_str);

    tmp_str = strtok(NULL, " ");
    if (tmp_str != NULL)
    {
      perror("ERROR: ");
      exit(1);
    }

    if (amount <= 0)
    {
      perror("Error: Negative amount! Invalid transfer!\n");
      exit(0);
    }
  }
  else
  {
    perror("Error: Empty Recipient! Can't conclude transfer\n");
    exit(0);
  }

  //if validade user 2;

  req_transfer_t transfer; //deduction
  transfer.account_id = user2;
  transfer.amount = amount;

  if (amount < MIN_BALANCE || amount > MAX_BALANCE)
    exit(1);
  if (user2 <= ADMIN_ACCOUNT_ID || user2 > MAX_BANK_ACCOUNTS || user2 == atoi(user1))
    exit(2);

  req_header_t user_info;
  user_info.account_id = atoi(user1);
  strcat(user_info.password, password);
  user_info.pid = pid;
  user_info.op_delay_ms = atoi(delay);

  tlv_request_t request;
  request.type = OP_TRANSFER;
  request.length = sizeof user_info + sizeof transfer;
  request.value.header = user_info;
  request.value.transfer = transfer;
  // Make send the struct
  return request;
}

tlv_request_t shutdown_server(char *id, char *password, char *delay, char *args, int pid)
{ //3
  //if validade user;
  if (atoi(id) != ADMIN_ACCOUNT_ID)
  {
    perror("ERROR: No permission for this operation");
    exit(1);
  }

  req_header_t user_info;
  user_info.account_id = atoi(id);
  strcat(user_info.password, password);
  user_info.pid = pid;
  user_info.op_delay_ms = atoi(delay);

  tlv_request_t request;
  request.type = OP_SHUTDOWN;
  request.length = sizeof user_info;
  request.value.header = user_info;
  // Make send the struct
  return request;
}