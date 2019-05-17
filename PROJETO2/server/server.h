#ifndef _SERVER_H_
#define _SERVER_H_

#include <stdbool.h>
#include "../constants.h"
#include "../types.h"
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
#include <signal.h>
#include <stdio.h>

void print_usage(FILE * stream, char * progname);
bool account_exists(tlv_request_t request);
bool check_hash(char *password, char *salt, char *desired_hash);
void sigint_handler(int sig);
char * hashingFunc(char *password);
bank_account_t accounts[MAX_BANK_ACCOUNTS];




#endif  // _SERVER_H_
