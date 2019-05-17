#include <stdio.h>
#include "thread_function.h"
#include "server.h"

void *thread_function(void *args)
{
    char fifo_user_name[USER_FIFO_PATH_LEN];
    int fifo_user;

    tlv_request_t request;
    tlv_reply_t reply;
    
    while (!CLOSE_FIFO_SERVER)
    {

        if (q.size > 0 ) //queue is global variable // semaphore is
        {
            printf("\n Entrou ca do thread %d",*((int *)args));
            sem_wait(&full);

            request = q.front->data;
            sleep(request.value.header.op_delay_ms);
            pop(&q);
            memset(fifo_user_name, 0, USER_FIFO_PATH_LEN);
            //open fifo_user
            sprintf(fifo_user_name, "/tmp/secure_%d", request.value.header.pid); // int to string

            fifo_user = open(fifo_user_name, O_WRONLY | O_NONBLOCK);

            if (fifo_user < 0)
            {
                printf("Error: Failed to open user fifo user: %s\n", fifo_user_name);
            }
            else
            {
                printf("fifo user: %s opened\n", fifo_user_name);
            }

            //process request

            reply = process_reply(request);

            write(fifo_user, &reply, sizeof reply);
            printf("Message sent to fifo user: %s \n", fifo_user_name);
            sem_post(&empty);

            close(fifo_user);
        }
    }
    return NULL;
}