#include "server.h"

int main(int argc, char *argv[])
{

  int fifo_server;
  int fifo_user;
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

  struct sigaction action;

  action.sa_handler = sigint_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaction(SIGINT, &action, NULL);

  char *a = hashing_func(argv[2]);
  char hash[HASH_LEN + 1];
  char saltes[SALT_LEN + 1];

  char *tmp;
  tmp = strtok(a, " ");
  strcpy(hash, tmp);
  tmp = strtok(NULL, " ");
  strcpy(saltes, tmp);

  // CRIAR CONTA ADMINISTRADOR
  struct bank_account admin_account;
  admin_account.account_id = ADMIN_ACCOUNT_ID;
  admin_account.balance = 0;

  strcpy(admin_account.hash, hash);   //output is hashed password
  strcpy(admin_account.salt, saltes); //the Salt

  accounts[ADMIN_ACCOUNT_ID] = admin_account; //SET ADMIN ACCOUNT TO POSX 0

  //criar fifo secure_srv
  if (mkfifo(SERVER_FIFO_PATH, 0660) != 0)
  {
    fprintf(stderr, "Error creating secure_srv fifo. Restart the program\n");
    unlink(SERVER_FIFO_PATH);
    return -3;
  }
  fifo_server = open(SERVER_FIFO_PATH, O_RDONLY | O_NONBLOCK);
  if (fifo_server == -1)
  {
    printf("Error opening FIFO. \n");
    return -1;
  }
  struct tlv_request request;
  struct tlv_reply reply;

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

  //read info from user
  char fifo_user_name[USER_FIFO_PATH_LEN];
  char user_pid[WIDTH_ID];

  while (1)
  {
    if (read(fifo_server, &request, sizeof(request)) > 0)
    {
      memset(fifo_user_name, 0, USER_FIFO_PATH_LEN);
      printf("\nPID:%i\n", request.value.header.pid);
      printf("DELAY:%i\n", request.value.header.op_delay_ms);
      printf("account_id:%i\n", request.value.header.account_id);

      //open fifo_user
      sprintf(fifo_user_name, "/tmp/secure_%d", request.value.header.pid); // int to string

      fifo_user = open(fifo_user_name, O_WRONLY | O_NONBLOCK);

      if (fifo_user < 0)
      {
        printf("Error: Failed to open user fifo user: %s\n", fifo_user_name);
      }
      else
      {
        printf("fifo user: %s opened\n", fifo_user_name);
      }

      //process request

      reply = process_reply(request);

      write(fifo_user, &reply, sizeof reply);
      printf("Message sent to fifo user: %s \n", fifo_user_name);

      close(fifo_user);
    }
  }

  close(fifo_server);
  unlink(SERVER_FIFO_PATH);
  return 0;
}

char *hashing_func(char *password)
{
  FILE *command_hash;
  srand(time(NULL)); //Randomize time
  char *values = malloc(65 * 2 * sizeof(char));
  //GENERATE SALT
  int MAX_NUMBER = pow(2, 30);
  int salt_number = rand() % MAX_NUMBER + 1;

  //sha256
  char salt[SALT_LEN + 1];

  //code
  char code[30] = "echo -n ";
  char output[HASH_LEN + 1];
  sprintf(salt, "%i", salt_number);
  strcpy(values, salt);

  strcat(code, password); // password
  strcat(code, " ");
  strcat(code, salt); //salt
  strcat(code, " | sha256sum");
  command_hash = popen(code, "r");
  fgets(output, HASH_LEN + 1, command_hash); //read 64 bytes

  strcat(values, " ");
  strcat(values, output);
  return values;
}

bool check_hash(char *password, char *salt, char *desired_hash)
{
  FILE *command_hash;
  //code
  char code[30] = "echo -n ";
  char hash[HASH_LEN + 1];

  strcat(code, password); // echo -n "password"
  strcat(code, " ");
  strcat(code, salt); //echo -n "password" "salt"
  strcat(code, " | sha256sum");
  command_hash = popen(code, "r");
  fgets(hash, HASH_LEN + 1, command_hash); //read 64 bytes

  if (strcmp(hash, desired_hash) == 0) //same password
    return true;
  else
    return false;
}

bool account_exists(tlv_request_t request)
{

  int id = request.value.header.account_id;

  if (accounts[id].hash[0] == '\0') //Checks if its null
  {
    printf("ACCOUNT DOESN'T EXIST\n");
    return false;
  }
  else
  {
    printf("ACCOUNT DOES EXIST\n");

    return true;
  }
}
bool check_login(tlv_request_t request)
{
  int id = request.value.header.account_id;
  char *password = request.value.header.password;
  char *hash = accounts[id].hash;
  char *salt = accounts[id].salt;
  if (check_hash(password, salt, hash))
    return true;
  else
    return false;
}

void sigint_handler(int sig)
{

  static int in = 0;

  if (sig == 2 && in == 0) // Received control+c signal askign user to leave
  {
    in = 1;
    char c;
    printf("Are you sure you want to terminate(Y/N) ");
    c = getchar();
    if (c == 'y' || c == 'Y')
    {
      unlink(SERVER_FIFO_PATH);
      exit(2);
    }
  }
  else
    in = 0;
}

void print_usage(FILE *stream, char *progname)
{
  fprintf(stream, "usage: %s <nr_bank_offices_to_create> <password>\n", progname);
}
