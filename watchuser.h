#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utmpx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void init_thread();
void *watch_thread();
