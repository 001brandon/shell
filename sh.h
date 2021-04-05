#include "get_path.h"
#include "linked.h"

int pid;
char *which(char *command, struct pathelement *pathlist);
char *where(char *command, struct pathelement *pathlist, int pathNum);
//char *whereNew(char *command, struct pathelement *p, int pathNum)
//void list(char *dir);
void printenv(char **envp);
void showprompt();
void cd(const char* path);
void list(const char *path);

#define PROMPTMAX 64
#define MAXARGS   16
#define MAXLINE   128
