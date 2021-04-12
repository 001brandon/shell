#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utmpx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_char.h"
pthread_mutex_t lock;
void init_thread();
void *watch_thread();
void checkuser(char *username);
void checkList();
void checkLogOff(char *username);