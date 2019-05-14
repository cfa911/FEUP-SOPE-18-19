#include <stdio.h>
#include "thread_function.h"


void * thread_function(void * arg){
    printf("thread id: %id\n", *((int *)arg));
    return NULL;
}