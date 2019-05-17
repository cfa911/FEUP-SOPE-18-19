#include <stdio.h>
#include "../types.h"
#include "server.h"
#include "account.h"
#include "../constants.h"


tlv_reply_t create_acount(tlv_request_t request){


}

tlv_reply_t check_balance(tlv_request_t request){

tlv_reply_t reply;
rep_value_t value;
rep_header_t header;
rep_balance_t balance;

if(account_exists(request)){
  reply.type = RC_OK;
  //balance.balance = accounts[request.value.header.account_id].balance;
 //header = request.value.header;
  value.header = header;
  value.balance = balance;
  reply.value = value;
  reply.length = sizeof reply;
  }
  // else if(request.value.header.account_id == ADMIN_ACCOUNT_ID){

  // } else{

  // }
    return reply;

}

tlv_reply_t make_transfer(tlv_request_t request){



}

tlv_reply_t server_shutdown(tlv_request_t request){



}
