#include <stdio.h>
#include "../types.h"
#include "server.h"
#include "account.h"
#include "../constants.h"

tlv_reply_t create_acount(tlv_request_t request)
{
  //RC_OK
  //RC_OP_NALLOW  pedido realizado  por  um  cliente
  //RC_ID_IN_USE  conta  existente
  //RC_OTHER  erro  não  especificado
}

tlv_reply_t check_balance(tlv_request_t request)
{

  tlv_reply_t reply;
  rep_value_t value;
  rep_header_t header;
  rep_balance_t balance;

  if (account_exists(request))
  {
    reply.type = RC_OK;
    //balance.balance = accounts[request.value.header.account_id].balance;
    //header = request.value.header;
    value.header = header;
    value.balance = balance;
    reply.value = value;
    reply.length = sizeof reply;
  }
  else if (request.value.header.account_id == ADMIN_ACCOUNT_ID)
  {
    reply.type = RC_OP_NALLOW;
  }
  else
  {
    reply.type = RC_OTHER;
  }

  return reply;
}

tlv_reply_t make_transfer(tlv_request_t request)
{
  //RC_OK
  //RC_OP_NALLOW pedido realizado pelo administrador
  //RC_ID_NOT_FOUND a conta de destino não existe
  //RC_SAME_ID contas de origem e destino são a mesma
  //RC_NO_FUNDS saldo insuficiente
  //RC_TOO_HIGH saldo final passaria a ser demasiado elevado
  //RC_OTHER erro não especificado
}

tlv_reply_t server_shutdown(tlv_request_t request)
{

  tlv_reply_t reply;
  rep_value_t value;
  rep_header_t header;
  rep_shutdown_t shutdown;

  if (account_exists(request))
  {
    reply.type = RC_OP_NALLOW; //pedido realizado por um cliente
    value.header = header;
    value.shutdown = shutdown;
    reply.value = value;
    reply.length = sizeof reply;
  }
  else if (request.value.header.account_id == ADMIN_ACCOUNT_ID)
  {
    reply.type = RC_OK; //pedido realizado pelo administrador
  }
  else
  {
    reply.type = RC_OTHER;
  }

  return reply;
}
