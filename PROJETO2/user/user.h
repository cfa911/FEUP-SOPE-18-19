#ifndef _USER_H_
#define _USER_H_
#include "../types.h"
#include "waiting_thread.h"

void print_usage(FILE * stream, char * progname);
tlv_request_t create_account(char *user, char *password,char *delay,char *args,int pid);
tlv_request_t check_balance(char *user, char *password,char *delay,char *args,int pid);
tlv_request_t make_transfer(char *user1, char *password,char *delay,char *args,int pid);
tlv_request_t shutdown_server(char *user, char *password,char *delay,char *args,int pid);

#endif  // _USER_H_
