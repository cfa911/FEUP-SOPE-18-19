#include <stdio.h>
#include "bank_thread.h"
#include "../sope.h"
void *bank_thread(void *args)
{
    char fifo_user_name[USER_FIFO_PATH_LEN];
    int fifo_user;
    tlv_request_t request;
    tlv_reply_t reply;
    int ret_code;
    int operation;
    int fd = open(SERVER_LOGFILE, O_APPEND | O_WRONLY);
    int sem_value = 0;
    arg_struct_t thread_args = *(arg_struct_t *)args;
    int thread_id = thread_args.id;
    logBankOfficeOpen(fd, thread_id, pthread_self());
    logBankOfficeOpen(STDOUT_FILENO, thread_id, pthread_self());

    while (!CLOSE_FIFO_SERVER || q.size > 0)
    {

        if (q.size > 0) //queue is global variable // semaphore is
        {
            sem_wait(&full);
            sem_getvalue(&full, &sem_value);
            logSyncMechSem(fd, thread_id, SYNC_OP_SEM_WAIT, SYNC_ROLE_CONSUMER, request.value.header.pid, sem_value);
            logSyncMechSem(STDOUT_FILENO, thread_id, SYNC_OP_SEM_WAIT, SYNC_ROLE_CONSUMER, request.value.header.pid, sem_value);
            request = q.front->data;
            //usleep(request.value.header.op_delay_ms);
            pop(&q);

            memset(fifo_user_name, 0, USER_FIFO_PATH_LEN);
            //open fifo_user
            sprintf(fifo_user_name, "/tmp/secure_%d", request.value.header.pid); // int to string

            fifo_user = open(fifo_user_name, O_WRONLY);

            reply = process_reply(request);
            if (fifo_user < 0)
            {
                printf("Error: Failed to open user fifo user: %s\n", fifo_user_name);
                reply.value.header.ret_code = RC_USR_DOWN;
                //USR_DOWN
            }
            //process request
            logDelay(fd, thread_id, request.value.header.op_delay_ms);
            logDelay(STDOUT_FILENO, thread_id, request.value.header.op_delay_ms);

            sleep(request.value.header.op_delay_ms / 1000); //MAKES DELAY IN MS
            //usleep(request.value.header.op_delay_ms); //MAKES DELAY IN MS

            ret_code = reply.value.header.ret_code;
            operation = reply.type;
            if (ret_code == RC_OK)
            {
                switch (operation)
                {
                case OP_CREATE_ACCOUNT:
                    create_account(request, thread_id, &fd);
                    break;
                case OP_BALANCE:
                    break;
                case OP_TRANSFER:
                    make_transfer(request, thread_id, &fd);
                    break;
                case OP_SHUTDOWN:
                    server_shutdown(request, thread_id, &fd);
                    unlink(SERVER_FIFO_PATH);
                    break;

                default:
                    break;
                }
            }
            if (reply.value.header.ret_code != RC_USR_DOWN)
                write(fifo_user, &reply, sizeof reply);

            close(fifo_user);

            sem_post(&empty);
            sem_getvalue(&empty, &sem_value);
            logSyncMechSem(fd, thread_id, SYNC_OP_SEM_POST, SYNC_ROLE_CONSUMER, request.value.header.pid, sem_value);
            logSyncMechSem(STDOUT_FILENO, thread_id, SYNC_OP_SEM_POST, SYNC_ROLE_CONSUMER, request.value.header.pid, sem_value);
        }
    }

    logBankOfficeClose(fd, thread_id, pthread_self());
    logBankOfficeClose(STDOUT_FILENO, thread_id, pthread_self());
    close(fd);
    return NULL;
}

void create_account(tlv_request_t request, int thread_id, int *fd)
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
    const bank_account_t *account = &new_account;
    logAccountCreation(*fd, thread_id, account);
    logAccountCreation(STDOUT_FILENO, thread_id, account);

    pthread_mutex_init(&(mutex[id]), NULL);
    pthread_mutex_lock(&(mutex[id]));

}
void make_transfer(tlv_request_t request, int thread_id, int *fd)
{
    int id1 = request.value.header.account_id;
    int id2 = request.value.transfer.account_id;
    int amount = request.value.transfer.amount;
    accounts[id2].balance += amount;
    accounts[id1].balance -= amount;
}
void server_shutdown(tlv_request_t request, int thread_id, int *fd)
{
    chmod(SERVER_FIFO_PATH, S_IRUSR | S_IRGRP | S_IROTH);
    CLOSE_FIFO_SERVER = 1;
}