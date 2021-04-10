#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utmpx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "watchuser.h"
void init_thread(){
    pthread_t watch_pid;
    pthread_create(&watch_pid, NULL, watch_thread, "watch thread created");
}

void *watch_thread(){
    while(1){
    printf("EXECUTING\n");
    sleep(10);
    }
}