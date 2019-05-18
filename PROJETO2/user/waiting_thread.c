#include <stdio.h>
#include <unistd.h>
#include "waiting_thread.h"

void *waiting_thread(void *args)
{
    sleep(30);
    args = 30;
    return NULL;
}