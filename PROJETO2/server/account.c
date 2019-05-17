#include <stdio.h>
#include "../types.h"
#include "server.h"
#include "account.h"
#include "../constants.h"

tlv_reply_t process_reply(tlv_request_t request)
{
  tlv_reply_t reply;
  switch (request.type)
  {
  case OP_CREATE_ACCOUNT:
    return create_account(request);
    break;
  case OP_BALANCE:
    return check_balance(request);
    break;
  case OP_TRANSFER:
    return make_transfer(request);
    break;
  case OP_SHUTDOWN:
    return server_shutdown(request);
    break;
  default:
    return reply;
    break;
  }
}

tlv_reply_t create_account(tlv_request_t request)
{
  tlv_reply_t reply;
  rep_value_t value;
  rep_header_t header;
  int id = request.value.header.account_id;

  header.account_id = request.value.header.account_id;
  value.header = header;
  reply.length = sizeof(value);
  reply.value = value;
  reply.type = OP_CREATE_ACCOUNT;
  if (account_exists(request)) //RC_ID_IN_USE  conta  existente
  {
    header.ret_code = RC_ID_IN_USE;
  }
  else
  {
    if (check_hash(request.value.header.password, accounts[id].salt, accounts[id].hash))
    {
      if (id == ADMIN_ACCOUNT_ID)
      { //IS ADMIN
        header.ret_code = RC_OK;
      }
      else
      {
        header.ret_code = RC_OP_NALLOW;
      }
    }
    else
    {
      header.ret_code = RC_LOGIN_FAIL;
    }
  }
  return reply;
  //RC_OK
  //RC_OP_NALLOW  pedido realizado  por  um  cliente
  //RC_OTHER  erro  não  especificado
}

tlv_reply_t check_balance(tlv_request_t request)
{

  tlv_reply_t reply;
  rep_value_t value;
  rep_header_t header;
  rep_balance_t balance;
  reply.type = OP_BALANCE;
  int id = request.value.header.account_id;

  if (account_exists(request))
  {
    if (check_hash(request.value.header.password, accounts[id].salt, accounts[id].hash))
    {
      if (id == ADMIN_ACCOUNT_ID)
      {
        header.ret_code = RC_OP_NALLOW;
      }
      else
        header.ret_code = RC_OK;
    }
    else
    {
      header.ret_code = RC_LOGIN_FAIL;
    }
  }
  else
  {
    header.ret_code = RC_ID_NOT_FOUND;
  }

  header.account_id = id;
  balance.balance = accounts[id].balance;
  value.header = header;
  value.balance = balance;
  reply.value = value;
  reply.length = sizeof(reply); //CHANGE

  return reply;
}

tlv_reply_t make_transfer(tlv_request_t request)
{

  tlv_reply_t reply;
  rep_value_t value;
  rep_header_t header;
  rep_transfer_t transfer;
  reply.type = OP_TRANSFER;
  header.account_id = request.value.header.account_id;
  int id1 = request.value.header.account_id;
  int id2 = request.value.transfer.account_id;

  int balance_1;
  int balance_2;
  int amount = request.value.transfer.amount;

  tlv_request_t aux_request;
  aux_request.value.header.account_id = request.value.transfer.account_id;
  //RC_OK
  if (account_exists(request))
  { //check for user 1
    if (account_exists(aux_request))
    { //check for user 2
      if (check_hash(request.value.header.password, accounts[id1].salt, accounts[id1].hash))
      {
        if (id1 != ADMIN_ACCOUNT_ID)
        {
          if (id1 == id2)
          {
            header.ret_code = RC_SAME_ID;
          }
          else
          {
            balance_1 = accounts[id1].balance;
            balance_2 = accounts[id2].balance;

            if ((balance_1 - amount) < MIN_BALANCE)
            {
              header.ret_code = RC_NO_FUNDS;
            }
            else if ((balance_2 + amount) > MAX_BALANCE)
            {
              header.ret_code = RC_TOO_HIGH;
            }
            else{
              header.ret_code = RC_OK;
            }
          }
        }
        else
          header.ret_code = RC_OP_NALLOW;
      }
      else
        header.ret_code = RC_ID_NOT_FOUND;
    }
    else
      header.ret_code = RC_LOGIN_FAIL;
  }
  else
  {
    header.ret_code = RC_ID_NOT_FOUND;
  }
  transfer.balance = balance_2 + amount;
  value.header = header;
  value.transfer = transfer;
  reply.value = value;
  reply.length = sizeof(reply);
  //TODO:RC_OTHER erro não especificado

  return reply;
}

tlv_reply_t server_shutdown(tlv_request_t request)
{

  tlv_reply_t reply;
  rep_value_t value;
  rep_header_t header;
  rep_shutdown_t shutdown;
  reply.type = OP_SHUTDOWN;
  header.account_id = request.value.header.account_id;
  if (account_exists(request))
  {
    if (request.value.header.account_id == ADMIN_ACCOUNT_ID)
      header.ret_code = RC_OK; //pedido realizado por um admin
    else
      header.ret_code = RC_OP_NALLOW; //pedido realizado por um cliente
  }
  else
  {
    header.ret_code = RC_OTHER;
  }
  shutdown.active_offices; // TODO:HOW?? GET FROM SEMAPHORE??

  value.header = header;
  value.shutdown = shutdown;

  reply.value = value;
  reply.length = sizeof reply;
  return reply;
}
