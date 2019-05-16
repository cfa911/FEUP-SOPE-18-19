#ifndef _ACCOUNT_H_
#define _ACCOUNT_H_

tlv_reply_t create_acount(tlv_request_t request);
tlv_reply_t check_balance(tlv_request_t request);
tlv_reply_t make_transfer(tlv_request_t request);
tlv_reply_t server_shutdown(tlv_request_t request);

#endif  // _aCCOUNT_H_
