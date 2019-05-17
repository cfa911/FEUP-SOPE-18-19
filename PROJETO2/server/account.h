#ifndef _ACCOUNT_H_
#define _ACCOUNT_H_

#include "../types.h"
tlv_reply_t process_reply(tlv_request_t request);
tlv_reply_t request_create_account(tlv_request_t request);
tlv_reply_t request_check_balance(tlv_request_t request);
tlv_reply_t request_make_transfer(tlv_request_t request);
tlv_reply_t request_server_shutdown(tlv_request_t request);

void create_account(tlv_request_t request);
void make_transfer(tlv_request_t request);
void server_shutdown(tlv_request_t request);
#endif  // _aCCOUNT_H_
