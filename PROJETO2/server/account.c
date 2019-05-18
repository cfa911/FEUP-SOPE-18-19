#include <stdio.h>
#include "../types.h"
#include "server.h"
#include "account.h"
#include "../constants.h"

tlv_reply_t process_reply(tlv_request_t request)
{
  tlv_reply_t reply;
  switch ((int)request.type)
  {
  case OP_CREATE_ACCOUNT:
    reply = request_create_account(request);
    break;
  case OP_BALANCE:
    reply = request_check_balance(request);
    break;
  case OP_TRANSFER:
    reply = request_make_transfer(request);
    break;
  case OP_SHUTDOWN:
    reply = request_server_shutdown(request);
    break;
  }
  return reply;
}

tlv_reply_t request_create_account(tlv_request_t request)
{
  tlv_reply_t reply;
  rep_value_t value;
  rep_header_t header;
  int id1 = request.value.header.account_id;
  int id2 = request.value.create.account_id;

  header.account_id = id1;

  tlv_request_t tmp_request;
  tmp_request.value.header.account_id = id2;

  if (account_exists(tmp_request)) //RC_ID_IN_USE  conta  existente
  {
    header.ret_code = RC_ID_IN_USE;
  }
  else
  {
    if (check_hash(request.value.header.password, accounts[id1].salt, accounts[id1].hash))
    {
      if (id1 == ADMIN_ACCOUNT_ID) //IS ADMIN
      {
        if (id2 == ADMIN_ACCOUNT_ID)
          header.ret_code = RC_OP_NALLOW;
        else
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
  value.header = header;
  reply.length = sizeof(value);
  reply.value = value;
  reply.type = OP_CREATE_ACCOUNT;
  return reply;
  //RC_OK
  //RC_OP_NALLOW  pedido realizado  por  um  cliente
  //RC_OTHER  erro  não  especificado
}

tlv_reply_t request_check_balance(tlv_request_t request)
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

tlv_reply_t request_make_transfer(tlv_request_t request)
{

  tlv_reply_t reply;
  rep_value_t value;
  rep_header_t header;
  rep_transfer_t transfer;
  reply.type = OP_TRANSFER;
  header.account_id = request.value.header.account_id;
  int id1 = request.value.header.account_id;
  int id2 = request.value.transfer.account_id;

  uint32_t balance_1;
  uint32_t balance_2;
  uint32_t amount = request.value.transfer.amount;

  tlv_request_t aux_request;
  aux_request.value.header.account_id = request.value.transfer.account_id;
  //RC_OK
  if (account_exists(request))
  { //check for user 1
    if (account_exists(aux_request))
    { //check for user 2
      balance_1 = accounts[id1].balance;
      balance_2 = accounts[id2].balance;
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
            if (balance_1 <= amount)
            {
              header.ret_code = RC_NO_FUNDS;
            }
            else if (balance_2 + amount > MAX_BALANCE)
            {
              header.ret_code = RC_TOO_HIGH;
            }
            else
            {
              header.ret_code = RC_OK;
            }
          }
        }
        else
          header.ret_code = RC_OP_NALLOW;
      }
      else
        header.ret_code = RC_LOGIN_FAIL;
    }
    else
      header.ret_code = RC_ID_NOT_FOUND;
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

tlv_reply_t request_server_shutdown(tlv_request_t request)
{

  tlv_reply_t reply;
  rep_value_t value;
  rep_header_t header;
  rep_shutdown_t shutdown;
  reply.type = OP_SHUTDOWN;
  int id = header.account_id = request.value.header.account_id;
  if (account_exists(request))
  {
    if (check_hash(request.value.header.password, accounts[id].salt, accounts[id].hash))
    {
      if (request.value.header.account_id == ADMIN_ACCOUNT_ID)
        header.ret_code = RC_OK; //pedido realizado por um admin
      else
        header.ret_code = RC_OP_NALLOW; //pedido realizado por um cliente
    }
    else
    {
      header.ret_code = RC_LOGIN_FAIL;
    }
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

void create_account(tlv_request_t request)
{
  bank_account_t new_account;
  int id = new_account.account_id = request.value.create.account_id;

  new_account.balance = request.value.create.balance;
  char *password = request.value.create.password;

  char *a = hashing_func(password);
  char hash[HASH_LEN + 1];
  char salt[SALT_LEN + 1];

  char *tmp;
  tmp = strtok(a, " ");
  strcpy(salt, tmp);
  tmp = strtok(NULL, " ");
  strcpy(hash, tmp);

  strcpy(new_account.salt, salt);
  strcpy(new_account.hash, hash);
  accounts[id] = new_account;
}
void make_transfer(tlv_request_t request)
{
  int id1 = request.value.header.account_id;
  int id2 = request.value.transfer.account_id;
  int amount = request.value.transfer.amount;
  accounts[id2].balance += amount;
  accounts[id1].balance -= amount;
}
void server_shutdown(tlv_request_t request)
{
  chmod(SERVER_FIFO_PATH, S_IRUSR | S_IRGRP | S_IROTH);
  CLOSE_FIFO_SERVER = 1;
}