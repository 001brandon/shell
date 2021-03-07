#include "get_path.h"

int pid;
int where(char *command, struct pathelement *pathlist);
char *which(char *command, struct pathelement *pathlist);
void list(char *dir);
void printenv(char **envp);
void showprompt();

#define PROMPTMAX 64
#define MAXARGS   16
#define MAXLINE   128
