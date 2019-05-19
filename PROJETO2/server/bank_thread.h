#include "../types.h"
#include "server.h"
#include "../sope.h"
void * bank_thread(void * arg);
void create_account(tlv_request_t request, int thread_id, int *fd);
void make_transfer(tlv_request_t request, int thread_id, int *fd);
void server_shutdown(tlv_request_t request, int thread_id, int *fd);