#ifndef _USER_H_
#define _USER_H_

void print_usage(FILE * stream, char * progname);
void create_account(char *args, char *admin, char *password);
void check_balance(char *user,char *password);
void shutdown_server(char *admin, char *password);

#endif  // _USER_H_
