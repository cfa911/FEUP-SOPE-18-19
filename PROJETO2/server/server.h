#ifndef _SERVER_H_
#define _SERVER_H_

#include <stdbool.h>
#include "../types.h"

void print_usage(FILE * stream, char * progname);
bool validateAccount(tlv_request_t request);
void sigint_handler(int sig);
char * hashingFunc(char *password);
#endif  // _SERVER_H_