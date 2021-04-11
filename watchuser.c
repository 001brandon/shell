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
    pthread_create(&watch_user, NULL, watch_thread, "watch thread created");
}

void *watch_thread(){
    while(1){
    printf("EXECUTING\n");
    sleep(10);
    checkuser("test");
    }
}

void checkList(){
    struct nodeC *temp=headC;
    pthread_mutex_lock(&lock);
    while(temp!=NULL){
        checkuser(temp->data);
        temp=temp->next;
    }
    pthread_mutex_unlock(&lock);
}

void checkuser(char *username)
{
  struct utmpx *up;

  setutxent();			/* start at beginning */
  printf("Checking %s\n",username);
  while (up = getutxent() )	/* get an entry */
  {
    if ( up->ut_type == USER_PROCESS )	/* only care about users */
    {
    printf("USER FOUND: \n");
      if(strcmp(up->ut_user, username)==0) { /*check if user being watched matches*/
          printf("%s has logged on %s from %s \n", up->ut_user, up->ut_line, up ->ut_host);
      }
    }
  }
  return;
}