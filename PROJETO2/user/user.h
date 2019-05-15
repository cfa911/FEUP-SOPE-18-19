#ifndef _USER_H_
#define _USER_H_

void print_usage(FILE * stream, char * progname);
void create_account(char *args, char *admin, char *password);
void check_balance(char *user, char *password,char *delay,char *args,int pid);
void make_transfer(char *user1, char *password,char *delay,char *args,int pid);
void shutdown_server(char *admin, char *password);

#endif  // _USER_H_
