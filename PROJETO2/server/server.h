#ifndef _SERVER_H_
#define _SERVER_H_

#include "../constants.h"
#include "../types.h"
#include "bank_thread.h"
#include "account.h"
#include "queue.h"
#include "../sope.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <semaphore.h>

#define SHARED 0

void print_usage(FILE *stream, char *progname);
bool account_exists(tlv_request_t request);
bool check_hash(char *password, char *salt, char *desired_hash);
void sigint_handler(int sig);
char *hashing_func(char *password);

bank_account_t accounts[MAX_BANK_ACCOUNTS];
pthread_mutex_t mutex[MAX_BANK_ACCOUNTS];
struct Queue q; //queue struct
sem_t empty, full;
int CLOSE_FIFO_SERVER;

typedef struct arg_struct
{
    int id;
} arg_struct_t;

#endif // _SERVER_H_
