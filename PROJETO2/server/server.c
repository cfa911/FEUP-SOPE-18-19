#include "server.h"
#include "../log.c"

arg_struct_t args[MAX_BANK_OFFICES];
pthread_t threads[MAX_BANK_OFFICES];

int main(int argc, char *argv[])
{

  int fifo_server;
  CLOSE_FIFO_SERVER = 0;
  init(&q); // STARTS THE QUEUE
  // CHAIN OF REQUIREMENTS
  int fd = open(SERVER_LOGFILE, O_WRONLY | O_APPEND);

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
  strcpy(saltes, tmp);
  tmp = strtok(NULL, " ");
  strcpy(hash, tmp);

  // CRIAR CONTA ADMINISTRADOR
  struct bank_account admin_account;
  admin_account.account_id = ADMIN_ACCOUNT_ID;
  admin_account.balance = 0;

  strcpy(admin_account.hash, hash);   //output is hashed password
  strcpy(admin_account.salt, saltes); //the Salt

  accounts[ADMIN_ACCOUNT_ID] = admin_account; //SET ADMIN ACCOUNT TO POSX 0
  pthread_mutex_init(&(mutex[ADMIN_ACCOUNT_ID]), NULL);
  logSyncMech(fd, MAIN_THREAD_ID, SYNC_OP_SEM_INIT, SYNC_ROLE_PRODUCER, getpid());
  logSyncMech(STDOUT_FILENO, MAIN_THREAD_ID, SYNC_OP_SEM_INIT, SYNC_ROLE_PRODUCER, getpid());

  //criar fifo secure_srv
  if (mkfifo(SERVER_FIFO_PATH, 0660) != 0)
  {
    fprintf(stderr, "Error creating secure_srv fifo. Restart the program\n");
    unlink(SERVER_FIFO_PATH);
    return -3;
  }
  //

  fifo_server = open(SERVER_FIFO_PATH, O_RDONLY | O_NONBLOCK);
  if (fifo_server == -1)
  {
    printf("Error opening FIFO. \n");
    return -1;
  }
  struct tlv_request request;

  sem_init(&empty, SHARED, atoi(argv[1])); /* sem empty = atoi(argv[1]) */
  logSyncMechSem(fd, MAIN_THREAD_ID, SYNC_OP_SEM_INIT, SYNC_ROLE_PRODUCER, getpid(), atoi(argv[1]));
  logSyncMechSem(STDOUT_FILENO, MAIN_THREAD_ID, SYNC_OP_SEM_INIT, SYNC_ROLE_PRODUCER, getpid(), atoi(argv[1]));

  sem_init(&full, SHARED, 0); /* sem full = 0  */
  logSyncMechSem(fd, MAIN_THREAD_ID, SYNC_OP_SEM_INIT, SYNC_ROLE_PRODUCER, getpid(), 0);
  logSyncMechSem(STDOUT_FILENO, MAIN_THREAD_ID, SYNC_OP_SEM_INIT, SYNC_ROLE_PRODUCER, getpid(), 0);

  for (int i = 1; i <= atoi(argv[1]); i++)
  {
    args[i].id = i;
    usleep(2);
    if (pthread_create(&threads[i], NULL, &bank_thread, &args[i]))
    {
      printf("Error creating bank offices");
      exit(1);
    }
  }
  while (!CLOSE_FIFO_SERVER) //TILL SHUTDOWN
  {
    if (read(fifo_server, &request, sizeof(request)) > 0)
    {
      push(&q, request);
      int sem_value;

      sem_post(&full);
      sem_getvalue(&full, &sem_value);
      logSyncMechSem(fd, 0, SYNC_OP_SEM_WAIT, SYNC_ROLE_PRODUCER, request.value.header.pid, sem_value);
      logSyncMechSem(STDOUT_FILENO, 0, SYNC_OP_SEM_WAIT, SYNC_ROLE_PRODUCER, request.value.header.pid, sem_value);

      sem_wait(&empty);
      sem_getvalue(&full, &sem_value);
      logSyncMechSem(fd, 0, SYNC_OP_SEM_POST, SYNC_ROLE_PRODUCER, request.value.header.pid, sem_value);
      logSyncMechSem(STDOUT_FILENO, 0, SYNC_OP_SEM_POST, SYNC_ROLE_PRODUCER, request.value.header.pid, sem_value);
    }
  }

  for (int i = 1; i <= atoi(argv[1]); i++) // deletes threads/balcoes eletronicos
  {
    pthread_join(threads[i], NULL);
    usleep(1);
  }

  close(fifo_server);
  close(fd);
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
  if (strcmp(hash, desired_hash) == 0)     //same password
    return true;
  else
    return false;
}

bool account_exists(tlv_request_t request)
{

  int id = request.value.header.account_id;

  if (accounts[id].hash[0] == '\0') //Checks if its null
  {
    return false;
  }
  else
  {
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
