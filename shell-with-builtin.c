#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <glob.h>
#include <sys/wait.h>
#include "sh.h"


extern char **environ;

char	prefix[MAXLINE];

void sig_handler(int sig)
{
  fprintf(stdout, "\n>> ");
  fflush(stdout);
}
  
int
main(int argc, char **argv, char **envp)
{
	char	buf[MAXLINE];
	char    *arg[MAXARGS];  // an array of tokens
	char    *ptr;
        char    *pch;
	pid_t	pid;
	int	status, i, arg_no;
	


        signal(SIGINT, sig_handler);

	showprompt();
	while (fgets(buf, MAXLINE, stdin) != NULL) {
		if (strlen(buf) == 1 && buf[strlen(buf) - 1] == '\n')
		  goto nextprompt;  // "empty" command line

		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = 0; /* replace newline with null */
		// parse command line into tokens (stored in buf)
		arg_no = 0;
                pch = strtok(buf, " ");
                while (pch != NULL && arg_no < MAXARGS)
                {
		  arg[arg_no] = pch;
		  arg_no++;
                  pch = strtok (NULL, " ");
                }
		arg[arg_no] = (char *) NULL;

		if (arg[0] == NULL)  // "blank" command line
		  goto nextprompt;

		/* print tokens
		for (i = 0; i < arg_no; i++)
		  printf("arg[%d] = %s\n", i, arg[i]);
                */

                if (strcmp(arg[0], "pwd") == 0) { // built-in command pwd 
		  printf("Executing built-in [pwd]\n");
	          ptr = getcwd(NULL, 0);
                  printf("%s\n", ptr);
                  free(ptr);
	        }
		else
                if (strcmp(arg[0], "which") == 0) { // built-in command which
		  struct pathelement *p, *tmp;
                  char *cmd;
                    
		  printf("Executing built-in [which]\n");
		  if (arg[1] == NULL) {  // "empty" which
		    printf("which: Too few arguments.\n");
		    goto nextprompt;
                  }
			//yessss
		  p = get_path();
           /***/
		  tmp = p;
		  while (tmp) {      // print list of paths
		    //printf("path [%s]\n", tmp->element);
		    tmp = tmp->next;
                  }
           /***/

                  cmd = which(arg[1], p);
                  if (cmd) {
		    printf("%s\n", cmd);
                    free(cmd);
                  }
		  else               // argument not found
		    printf("%s: Command not found\n", arg[1]);

		  while (p) {   // free list of path values
		     tmp = p;
		     p = p->next;
		     free(tmp->element);
		     free(tmp);
                  }
	        } else if(strcmp(arg[0], "exit") == 0) {
				printf("Executing built-in [exit]\n");
				exit(0);
			}
		else if (strcmp(arg[0], "where") == 0) { // built-in command where
		  struct pathelement *p, *tmp;
                  //char **cmd;
				  char *cmd;
			int pathNum = 0;
		  printf("Executing built-in [where]\n");
		  if (arg[1] == NULL) {  // "empty" where
		    printf("where: Too few arguments.\n");
		    goto nextprompt;
                  }
			//yessss
		  p = get_path();
           /***/
		  tmp = p;
		  int i = 0;
		  while (tmp) {      // print list of paths
			//printf("path [%s]\n", tmp->element);
		    tmp = tmp->next;
			i++;
            }
           /***/
		   //printf("%i\n",i);
		   cmd = where(arg[1],p,pathNum);
			if(cmd) {
				while(cmd) {
					printf("%s\n", cmd);
                	free(cmd);
					pathNum++;
					cmd = where(arg[1],p,pathNum);
				}
			}
			else{
				printf("%s: Command not found\n", arg[1]);
			}
						/*
							int count = 1;
								cmd = where(arg[1], p);
								if(cmd != NULL){
									while(count <= cmd[0][0] - 48) {
										printf("%s\n",cmd[count]);
										//printf("%s\n",cmd[2]);
										count++;
									}
								}
								if (cmd) {
									for(i = 0; i < cmd[0][0] - 48; i++) {
										free(cmd[i]);
									}
								//printf("%s\n", cmd);
								free(cmd);
								}
						else               // argument not found
							printf("%s: Command not found\n", arg[1]);
							*/
		  while (p) {   // free list of path values
		     tmp = p;
		     p = p->next;
		     free(tmp->element);
		     free(tmp);
                  }
	    }	
		else if (strcmp(arg[0], "cd") == 0) {   
			printf("Executing built-in [cd]\n");
			if (arg[1] == NULL || arg[1][0] == '~') {  // goes to HOME directory
		    	cd(getenv("HOME"));
            }
			if (strcmp(arg[1],"-")==0){
				char *newstring=getcwd(NULL,0);
				cd(getenv("OLDPWD"));
				printf("%s\n",getenv("OLDPWD"));
				setenv("OLDPWD",newstring,1);
				free(newstring);
			} else {
			cd(arg[1]);
			}
		}
		else if (strcmp(arg[0], "list") == 0) {
			printf("Executing built-in [list]\n");
			if (arg[1] == NULL) {  // "empty" list defaults to .
		    	list(".");
		    	goto nextprompt;
            }
			else {
				list(arg[1]);			
			}
		}
		else if (strcmp(arg[0], "pid") == 0) {
			printf("Executing built-in [pid]\n");
			printf("The PID of this process is %d\n",getpid());
		    //goto nextprompt;
		} else if (strcmp(arg[0], "kill") == 0) {
			printf("Executing built-in [kill]\n");
			if (arg[1] == NULL) {  // "empty" kill
		    	printf("kill: Too few arguments.\n");
		    	goto nextprompt;
            } else if(arg[1][0]=='-'){
				if(arg[2]==NULL){
					printf("kill: Too few arguments. Did not include second argument for PID\n");
		    		goto nextprompt;
				}
				int sigUse=atoi(arg[2]);
				char *sigInput=arg[1];
				char* arg1=strtok(sigInput,"-");
				int option = atoi(arg1);
				if(option < 0 || option > 36) {
					printf("Not a valid option\n");
				}
				else{
					printf("Kill signal is: %i\n",option);
					kill(sigUse,option);
				}	
			}
			else {
				int sigUse=atoi(arg[1]);
				kill(sigUse,15);
			}
			
		    //goto nextprompt;
		}

		else if (strcmp(arg[0], "printenv") == 0) {
			printf("Executing built-in [printenv]\n");
			if(arg[1] != NULL && arg[2] != NULL) {
				printf("printenv: Too many arguments.\n");
				goto nextprompt;	
			}
			else {
				if(arg[1]==NULL){
					int index=0;
					while(environ[index]!=NULL){
						printf("%s\n",environ[index]);
						index++;
					}
				} 
				else {
					printf("Enviornment of %s is: %s\n",arg[1],getenv(arg[1]));
				}
			}
		}
		else if (strcmp(arg[0], "setenv") == 0) {  //come back to this update path linked list
			printf("Executing built-in [setenv]\n");
			const char* empty=" ";
			if(arg[1] != NULL && arg[2] != NULL && arg[3] != NULL) {
				printf("setenv: Too many arguments.\n");
			}
			else if(arg[1]==NULL){
				int index=0;
				while(environ[index]!=NULL){
					printf("%s\n",environ[index]);
					index++;
				} 
			}
			else if(arg[2]==NULL){
				setenv(arg[1],empty,1);  //I changed this to overwrite = 1 because that's how it works in tcsh
			} else{
				setenv(arg[1],arg[2],1);
			}
			

		}

		else if (strcmp(arg[0], "prompt") == 0) {
			printf("Executing built-in [prompt]\n");
			int leave=0;
			while(arg[1]==NULL && leave==0){
				printf("Enter a prompt: ");
				//scanf("%s",prefix);
				fgets(prefix,MAXLINE,stdin);
				if(strcmp(prefix,"\n")!=0){
					leave=1;
					prefix[strlen(prefix)-1]='\0';
				}
			}
			if(arg[1]!=NULL) {
				strcpy(prefix,arg[1]);
			}
			//goto nextprompt;



		}

		else {  // external command
		  if ((pid = fork()) < 0) {
			printf("fork error");
		  } else if (pid == 0) {		/* child */
			                // an array of aguments for execve()
	                char    *execargs[MAXARGS]; 
		        glob_t  paths;
                        int     csource, j;
			char    **p;
			struct pathelement *path, *tmp;

			/*for(int looper=0; looper<arg_no; looper++){
				execargs[looper]=malloc(strlen(arg[looper])+1);
				strcpy(execargs[looper], arg[looper]);
			}*/
			
			 //copy all the args into exec args
			//execargs[0] = malloc(strlen(arg[0])+1);
			//strcpy(execargs[0], arg[0]);  // copy command
		        j = 1;
		      for (i = 1; i < arg_no; i++) // check arguments
			  if (strchr(arg[i], '*') != NULL) { // wildcard!
			    csource = glob(arg[i], 0, NULL, &paths);
                            if (csource == 0) {
                              for (p = paths.gl_pathv; *p != NULL; ++p) {
                                execargs[j] = malloc(strlen(*p)+1);
				strcpy(execargs[j], *p);
				j++;
                              }
                           
                              globfree(&paths);
                            }
                          }
                        //execargs[j] = NULL;   // I dont know what this does

			i = 0;
			//char *const envp[2]={"PATH=/bin",NULL};
            for (i = 0; i < arg_no; i++)
			  printf("exec arg [%s]\n", arg[i]);
			if(arg[0][0]=='/'){
				execve(arg[0], arg, NULL);  //changed execargs to just args
			} else {

			}
			//execvp(execargs[0],execargs); //Use this to execute with respect to
			//here is a test
			printf("couldn't execute: %s", buf);
			exit(127);
		  }

		  /* parent */
		  if ((pid = waitpid(pid, &status, 0)) < 0)
			printf("waitpid error");
/**
                  if (WIFEXITED(status)) S&R p. 239 
                    printf("child terminates with (%d)\n", WEXITSTATUS(status));
**/
                }

           nextprompt:
		showprompt();
	}
	exit(0);
}

void showprompt(){
    char *cwd=getcwd(NULL,0);
	fprintf(stdout, "%s [%s] $: ",prefix,cwd);	/* print prompt (printf requires %% to print %) */
	free(cwd);
    fflush(stdout);
    return;
}
