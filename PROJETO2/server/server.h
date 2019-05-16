#ifndef _SERVER_H_
#define _SERVER_H_

#include <stdbool.h>

void print_usage(FILE * stream, char * progname);
bool validateAccount(tlv_request_t request);
void sigint_handler(int sig);

#endif  // _SERVER_H_
