#include "get_path.h"

char *which(char *command, struct pathelement *p)
{
  char cmd[64], *ch;
  int  found;

  found = 0;
  while (p) {       
    sprintf(cmd, "%s/%s", p->element, command);
    if (access(cmd, X_OK) == 0) {
      found = 1;
      break;
    }
    p = p->next;
  }
  if (found) {
    ch = malloc(strlen(cmd)+1);
    strcpy(ch, cmd);
    return ch;
  }
  else
    return (char *) NULL;
}

/*char **where(char *command, struct pathelement *p)
{
  char cmd[64];
  struct pathelement *temp;
  int i=0;
  temp=p;
  
  while (temp) {       
    sprintf(cmd, "%s/%s", p->element, command);
    if (access(cmd, X_OK) == 0) {
      i++;
    }
    temp = temp->next;
  }
  char **ch=malloc(sizeof(char *)*i+1);
  i=0;
  while (p) {       
    sprintf(cmd, "%s/%s", p->element, command);
    if (access(cmd, X_OK) == 0) {
      printf("I AM HERE \n");
      ch[i]=malloc(sizeof(char)*strlen(cmd)+1);
      strcpy(ch[i],cmd);
      i++;
    }
    p = p->next;
  }

  if (i !=0) {
    return ch;
  }
  else
    return (char **) NULL;
}
*/

int where(char *command, struct pathelement *p)
{
  char cmd[64], *ch;
  int  found;

  found = 0;
  while (p) {       
    sprintf(cmd, "%s/%s", p->element, command);
    if (access(cmd, X_OK) == 0) {
      found = 1;
      printf("%s\n",cmd);
    }
    p = p->next;
  }
  return found;
}