#include <stdio.h>
#include "thread_function.h"
#include "server.h"

void *thread_function(void *args)
{
    char fifo_user_name[USER_FIFO_PATH_LEN];
    int fifo_user;
    tlv_request_t request;
    tlv_reply_t reply;
    int ret_code;
    int operation;

    while (!CLOSE_FIFO_SERVER)
    {

        if (q.size >= 1) //queue is global variable // semaphore is
        {
            sem_wait(&full);

            request = q.front->data;

            printf("\nRequest id: %d", request.value.header.account_id);
            printf("\nRequest password: %s", request.value.header.password);
            printf("\nRequest PID: %d", request.value.header.pid);
            printf("\nRequest Delay: %d\n\n", request.value.header.op_delay_ms);
            //usleep(request.value.header.op_delay_ms);
            pop(&q);

            memset(fifo_user_name, 0, USER_FIFO_PATH_LEN);
            //open fifo_user
            sprintf(fifo_user_name, "/tmp/secure_%d", request.value.header.pid); // int to string

            fifo_user = open(fifo_user_name, O_WRONLY | O_NONBLOCK);
            if (fifo_user < 0)
            {
                printf("Error: Failed to open user fifo user: %s\n", fifo_user_name);
            }
            //process request

            reply = process_reply(request);
            sleep(request.value.header.op_delay_ms/1000); //MAKES DELAY IN MS
            ret_code = reply.value.header.ret_code;
            operation = reply.type;
            if (ret_code == RC_OK)
            {
                switch (operation)
                {
                case OP_CREATE_ACCOUNT:
                    create_account(request);
                    break;
                case OP_BALANCE:
                    break;
                case OP_TRANSFER:
                    make_transfer(request);
                    break;
                case OP_SHUTDOWN:
                    break;
                default:
                    break;
                }
            }

            write(fifo_user, &reply, sizeof reply);
            sem_post(&empty);

            close(fifo_user);
        }
    }
    return NULL;
}