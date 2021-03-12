#include "get_path.h"

char *which(char *command, struct pathelement *p)
{
  char cmd[64], *ch, founder[64];
  int  found;

  found = 0;
  while (p) {       
    sprintf(cmd, "%s/%s", p->element, command);
    if (access(cmd, X_OK) == 0) {
      found = 1;
      //printf("%s\n",cmd);
      strcpy(founder,cmd);
      break;
    }
    p = p->next;
  }
  if (found) {
    ch = malloc(strlen(founder)+1);
    strcpy(ch, founder);
    return ch;
  }
  else
    return (char *) NULL;
}

char *where(char *command, struct pathelement *p, int pathNum) {
  char cmd[64], *ch, founder[64];
  int  found;
  int correctOnes = 0;

  found = 0;
  while (p) {       
    sprintf(cmd, "%s/%s", p->element, command);
    if (access(cmd, X_OK) == 0 && correctOnes != pathNum) {
      //printf("%s\n",cmd);
      strcpy(founder,cmd);
      correctOnes++;
    }
    else if (access(cmd, X_OK) == 0 && correctOnes == pathNum) {
      found = 1;
      //printf("%s\n",cmd);
      strcpy(founder,cmd);
      break;
    }
    p = p->next;
  }
  if (found) {
    ch = malloc(strlen(founder)+1);
    strcpy(ch, founder);
    return ch;
  }
  else
    return (char *) NULL;
}

/*
char **where(char *command, struct pathelement *p)
{
  char cmd[64], **ch, founder[64];
  int  found;
  int i = 0;
  struct pathelement *tmp = p;

  found = 0;
  while (tmp) {       
    sprintf(cmd, "%s/%s", tmp->element, command);
    if (access(cmd, X_OK) == 0) {
      i++;
    }
    tmp = tmp->next;
  }
  //printf("number of paths: %i\n",i);
        ch = (char **)malloc(i*sizeof(char *)+2);
        for(int j = 0; j < i; j++) {
          ch[j] = (char *)malloc(64 * sizeof(char)+1);
        }
  //ch[0] = malloc(sizeof(char)+1);
  char *rows = malloc(64 * sizeof(char)+1);
  rows[0] = i + '0';
      //strcpy(ch[0],rows);  //The first row of ch contains the number of rows
  ch[0] = rows;
  //ch[0] = rows;
  //printf("rows equals: %s\n",ch[0]);
  i = 1;
  while (p) {       
    sprintf(cmd, "%s/%s", p->element, command);
    if (access(cmd, X_OK) == 0) {
      //printf("%s\n",cmd);
      strcpy(founder,cmd);
      found = 1;
          //ch[i] = malloc(strlen(founder) * sizeof(char)+1);
          //strcpy(ch[i],founder);
          //ch[i] = founder;
          for(int j = 0; j < (int)strlen(founder); j++) {
            ch[i][j] = founder[j];
          }
      i++;
      //break;
    }
    p = p->next;
  }
  if (found) {
    //ch = malloc(strlen(founder)+1);
    //strcpy(ch, founder);
    return ch;
  }
  else
    return (char **) NULL;
}
*/