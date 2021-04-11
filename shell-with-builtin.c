#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <glob.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "sh.h"

extern char **environ;

char	prefix[MAXLINE];

int calledFg = 0;

int fgNum;

int shellpid;

int noclobberVal=0;

int threadCreate=0;


struct node *temp, *n;

void sig_handler(int sig)
{
	if(sig==SIGINT){
		printf("current foregrounded process: %d\n",tcgetpgrp(0));
		if(shellpid == tcgetpgrp(0)) {
			printf("the shell is foregrounded, will not kill shell\n");
		}
		else {
			kill(tcgetpgrp(0),2);
		}
		if(calledFg) {
			printf("called fg\n");
			temp = head;
			for (int i = 0; i < fgNum - 1;i++) {
				temp = temp->next;
			}
			kill(temp->data,2);
			calledFg = 0;
		}
		//kill(0,SIGINT);
	} 
	//printf("\n");
  	//showprompt();
}

int status;

void reapChildNoHang(void);
void changeForegroundProcess(pid_t);
int checkForRedirectsAndModifyArgNo(int, char**);
  
int main(int argc, char **argv, char **envp) {
	char	buf[MAXLINE];
	char    *arg[MAXARGS];  // an array of tokens
	char *argLeft[MAXARGS]; //an array of tokens on left side of pipe
	char *argRight[MAXARGS]; //an array of tokens on right side of pipe
	char    *ptr;
	char *usrInput;
        char    *pch;
	pid_t	pid;
	int i, arg_no;
	int *pid_list=malloc(sizeof(int));
	int childPID;
	int isBackground=0;
	shellpid = getpid();
	int stdin_save;
	int stdout_save;
	int stderr_save;
	setpgid(shellpid,shellpid);
	head=NULL;
	n=head;
	signal(SIGTSTP, SIG_IGN); 
	signal(SIGINT, sig_handler);
	signal(SIGTERM, SIG_IGN);
	char *initPWD=getcwd(NULL,0);
	setenv("OLDPWD",initPWD,1);
	free(initPWD);
	int fd[2];
	int pipeFound=0;
	pid_t pipePid;
	int isLeftSide;

	RESTART:
	showprompt();
	while ((usrInput=fgets(buf, MAXLINE, stdin)) != NULL) {
		isLeftSide = 0;
		calledFg = 0;
		pipeFound=0;
		 //save filestreams so we can reopen later
		stdin_save = dup(0);
		stdout_save = dup(1);
		stderr_save = dup(2);
		dup2(stdin_save,0);
		dup2(stdout_save,1);
		dup2(stderr_save,2);
		//reaps child at start of every command
		reapChildNoHang();
		if (strlen(buf) == 1 && buf[strlen(buf) - 1] == '\n')
		  goto nextprompt;  // "empty" command line

		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = 0; /* replace newline with null */
		// parse command line into tokens (stored in buf)
		arg_no = 0;
                pch = strtok(buf, " ");
				int arg_no2=0;
                while (pch != NULL && arg_no < MAXARGS) {
					if(strcmp(pch,"|")==0|strcmp(pch,"|&")==0){
						pipeFound=1;
						pch = strtok (NULL, " ");
						pipe(fd);
						} else {
							/*close(fd[0]);
							close(1);
							dup(fd[1]);*/


					}
					if(!pipeFound){
					argLeft[arg_no] = pch;
					arg_no++;
					}
					else{
						argRight[arg_no2]=pch;
						arg_no2++;
					}
					pch = strtok (NULL, " ");
                }
		argLeft[arg_no] = (char *) NULL;
		argRight[arg_no2]= (char *) NULL;

		if(pipeFound) {
			//child will be left side, parent will be right side
			//child will redirect stdoutput to stdinput of parent
			//parent waits for child to finish (blocking) and then starts
			pipePid = fork();
			if(pipePid < 0) {
				printf("error forking pipe\n");
				exit(5);
			}
			if(pipePid == 0) {
				isLeftSide = 1;
				memcpy(arg,argLeft,sizeof(argLeft));  //assign the arg array to the array of the left side
				//close(1);
				//dup(fd[1]);
				close(fd[0]);
				dup2(fd[1],STDOUT_FILENO);
				close(fd[1]);
			}
			else{
				isLeftSide = 0;
				arg_no = arg_no2;		//assign the arg count variable to the arg count of the right side
				memcpy(arg,argRight,sizeof(argRight));  //assign the arg array to the array of the right side
				setpgid(pipePid,pipePid);
				tcsetpgrp(0,pipePid);
				signal(SIGTTOU, SIG_IGN);
				tcsetpgrp(1,pipePid);
				signal(SIGTTOU, SIG_IGN);
				tcsetpgrp(2,pipePid);
				signal(SIGTTOU, SIG_IGN);
				kill(- pipePid,SIGCONT);
				waitpid(pipePid,&status,0);
				tcsetpgrp(0, getpid());
				tcsetpgrp(1, getpid());
				tcsetpgrp(2, getpid());
				signal(SIGTTOU, SIG_DFL);
				close(fd[1]);
				dup2(fd[0],STDIN_FILENO);
				close(fd[0]);
				waitpid(pipePid,&status,0);
				printf("child has finished with status %d and exited status %d, will now execute right side of pipe\n",WEXITSTATUS(status), WIFEXITED(status));
				//close(0);
				//dup(fd[0]);
			}
		}
		else{
			memcpy(arg,argLeft,sizeof(argLeft));
		}
		printf("isLeftSide: %d\n",isLeftSide);

		if (arg[0] == NULL) { // "blank" command line
			if(isLeftSide ==1) {
				printf("left side blank\n");
				exit(50);
			}
		  	goto nextprompt;
		}

		/* print tokens
		for (i = 0; i < arg_no; i++)
		  printf("arg[%d] = %s\n", i, arg[i]);
                */


		//check for redirects
		arg_no = checkForRedirectsAndModifyArgNo(arg_no, arg);

            if (strcmp(arg[0], "pwd") == 0) { // built-in command pwd 
				printf("Executing built-in [pwd]\n");
				ptr = getcwd(NULL, 0);
				printf("%s\n", ptr);
				free(ptr);
	        }
			else if (strcmp(arg[0], "fg") == 0) { 
				printf("Executing built-in [fg]\n");
				calledFg = 1;
				if(head != NULL) {
					if (arg[1] == NULL) { // "empty" fg defaults to first backgrounded job
						fgNum = 1;
						printf("sending job number 1 to foreground\n");
						changeForegroundProcess((pid_t)head->data);
						delete(head->data);
					}
					else{
						fgNum = atoi(arg[1]);
						printf("sending job number %d to foreground\n",fgNum);
						temp = head;
						for(int i = 0; i < fgNum - 1;i++){
							temp = temp->next;
						}
						waitpid(temp->data,&status,0);
						delete(temp->data);
					}
				}
				else{
					printf("no backgrounded jobs!\n");
				}
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
			for(int testcounter = 1; testcounter < arg_no; testcounter++){
				cmd = which(arg[testcounter], p);
				if (cmd) {
					printf("%s\n", cmd);
					free(cmd);
					}
				else {              // argument not found
					printf("%s: Command not found\n", arg[testcounter]);
				}
				p=get_path();
				testcounter++;
			}

		  while (p) {   // free list of path values
		     tmp = p;
		     p = p->next;
		     free(tmp->element);
		     free(tmp);
                  }
	        } else if(strcmp(arg[0], "exit") == 0) {
				printf("Executing built-in [exit]\n");
				freeall();
				freeallC();
				free(pid_list);
				pthread_cancel(watch_user);
				pthread_join(watch_user,NULL);
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
		   int testcase=1;
		   for(int testcase = 1; testcase < arg_no; testcase++) {
			cmd = where(arg[testcase],p,pathNum);
			if(cmd) {
				while(cmd) {
					printf("%s\n", cmd);
                	free(cmd);
					pathNum++;
					cmd = where(arg[testcase],p,pathNum);
				}
			}
			else{
				printf("%s: Command not found\n", arg[1]);
				//break;
			}
			while (p) {   // free list of path values
		     tmp = p;
		     p = p->next;
		     free(tmp->element);
		     free(tmp);
                  }
			   testcase++;
			   	p=get_path();
				pathNum=0;
		   }
		   

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
			else if (strcmp(arg[1],"-")==0){
				char *newstring=getcwd(NULL,0);
				cd(getenv("OLDPWD"));
				printf("%s\n",getenv("OLDPWD"));
				setenv("OLDPWD",newstring,1);
				free(newstring);
			} 
			else {
			cd(arg[1]);
			}
		}
		else if (strcmp(arg[0], "list") == 0) {
			char file[255];
			printf("Executing built-in [list]\n");
			if (arg[1] == NULL || arg[1][0] != '/') {  // "empty" list defaults to .
			    struct dirent *files;
				DIR *directory=opendir(".");
				while ((files = readdir(directory)) != NULL) {
					printf ("%s\n", files->d_name);
				}
				closedir(directory);
		    	//list(".");
            }
			else {
				int testcounter=1;
				for(int testcounter = 1; testcounter < arg_no; testcounter++) {
					struct dirent *files;
					DIR *directory=opendir(arg[testcounter]);
					while ((files = readdir(directory)) != NULL) {
						printf ("%s\n", files->d_name);
					}
					closedir(directory);
					//list(arg[testcounter]);
				}	
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
			if(arg_no > 2) {
				printf("printenv: Too many arguments.\n");
				goto nextprompt;	
			}
			else if(arg_no < 2){
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
		else if (strcmp(arg[0], "setenv") == 0) {  //come back to this update path linked list
			printf("Executing built-in [setenv]\n");
			const char* empty=" ";
			if(arg_no > 3) {
				printf("setenv: Too many arguments.\n");
			}
			else if(arg_no == 1){
				int index=0;
				while(environ[index]!=NULL){
					printf("%s\n",environ[index]);
					index++;
				} 
			}
			else if(arg_no == 2){
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

		else if (strcmp(arg[0], "noclobber") == 0) {
			printf("Executing built-in [noclobber]\n");
			noclobberVal=!noclobberVal;
			printf("%d\n",noclobberVal);
		}
		else if(strcmp(arg[0], "watchuser")==0){
			if(arg[1]!=NULL && arg[2]!=NULL && strcmp(arg[2],"off")==0){
				if(threadCreate==0){
				init_thread();
				threadCreate=1;
			}
				printf("Executing built- in [watchuser] removing user %s\n",arg[2]);
				pthread_mutex_lock(&lock);
				deleteC(arg[1]);
				pthread_mutex_unlock(&lock);				
			} else if(arg[2]==NULL && arg[1]!=NULL){
				if(threadCreate==0){
				init_thread();
				threadCreate=1;
			}
				printf("Executing built-in [watchuser] for user %s\n",arg[1]);
				pthread_mutex_lock(&lock);
				insertC(arg[1]);
				pthread_mutex_unlock(&lock);
			} else{
				printf("Invalid watchuser arguments\n");
			}
		}
		else {  // external command
		if(strcmp(arg[arg_no-1],"&")==0){
				arg_no=arg_no-1;
				isBackground=1;
		}
		//Use redirect here to output file
		//Check parent vs child process

		  if ((pid = fork()) < 0) {
			printf("fork error");
		  } else if (pid == 0) {		/* child */
			                // an array of arguments for execve()


	            char    *execargs[MAXARGS]; 
		        glob_t  paths;
                        int     csource, j;
			char    **p;
			struct pathelement *path, *tmp;

			//check for child process redirects
			arg_no = checkForRedirectsAndModifyArgNo(arg_no,arg);

			for(int looper=0; looper<arg_no; looper++){
				execargs[looper]=malloc(strlen(arg[looper])+1);
				strcpy(execargs[looper], arg[looper]);
			}
		    j = 1;
		    for (i = 1; i < arg_no; i++) // check arguments
			if (strchr(arg[i], '*') != NULL) { // wildcard!
			    csource = glob(arg[i], 0, NULL, &paths);
				if (csource == 0) {
					for (p = paths.gl_pathv; *p != NULL; ++p) {
						execargs[i] = malloc(strlen(*p)+1);
						strcpy(execargs[i], *p);
						//printf("execargs[j] is: %s ",execargs[i]);
						i++;
						//printf("p is : %s\n"/*,execargs[j]*/,*p);
					}     
					globfree(&paths);
				}
				else{
					for(int looper=0; looper<arg_no; looper++){
						free(execargs[looper]);
					}
					printf("%s: No match.\n",arg[0]);
					exit(-8);
				}
				arg_no = i-1;
				execargs[arg_no+1] = NULL;
            }
			execargs[i] = NULL;
			
                        //execargs[j] = NULL;   // I dont know what this does
			i = 0;
			//char *const envp[2]={"PATH=/bin",NULL};
            for (i = 0; i < arg_no; i++)
			  printf("exec arg [%s]\n", execargs[i]);
			if(arg[0][0]=='/'){
				execve(execargs[0], execargs, NULL);  //changed execargs to just args
			} else {
				char *cmd;
				struct pathelement *p, *tmp;
				p = get_path();


				cmd = which(execargs[0], p);
				if(cmd){
					printf("CMD IS: %s\n",cmd);
				}
				else{              // argument not found
					printf("%s: Command not found\n", execargs[0]);
					exit(-8);
				}
				while (p) {   // free list of path values
					tmp = p;
					p = p->next;
					free(tmp->element);
					free(tmp);
						}
				execargs[0]=realloc(execargs[0],strlen(cmd)+1);
				strcpy(execargs[0],cmd);
				free(cmd);

				execve(execargs[0],execargs,NULL);
				}
			printf("couldn't execute: %s", buf);
			exit(127);
		  }

		  /* parent */
		  printf("parent pid: %d\n",getpid());
		  printf("fd group: %d\n",tcgetpgrp(0));
		  childPID = pid;
		  setpgid(pid,pid);
		  printf("child pid: %d\n",pid);
		  if(isBackground==1){
			  insert(pid);
			  isBackground=0;
			  tcsetpgrp(0,shellpid);
			  tcsetpgrp(1,shellpid);
			  tcsetpgrp(2,shellpid);
		  }
		  else {
			changeForegroundProcess(pid);
		  }
		}
			
			
           nextprompt:
		   if(isLeftSide) {
			   dup2(stdin_save,0);
				dup2(stdout_save,1);
				dup2(stderr_save,2);
				printf("hey im done\n");
			   close(fd[1]);
			   close(1);
			   close(0);
			   close(fd[0]);
			   exit(50);
		   }
		   //reopen filestreams
		   dup2(stdin_save,0);
		   dup2(stdout_save,1);
		   dup2(stderr_save,2);
		//showprompt();
			reapChildNoHang();
			display(n);
			char *cwd=getcwd(NULL,0);
			fprintf(stdout, "%s [%s] $: ",prefix,cwd);	/* print prompt (printf requires %% to print %) */
			free(cwd);
			fflush(stdout);
	}
	if(usrInput==NULL){
		printf("^D\n");
    	printf("Use \"exit\" to leave shell.\n");
		freopen("/dev/tty", "rw", stdin);
		goto RESTART;
	}
	exit(0);
}

void showprompt(){
	reapChildNoHang();
	display(n);
    char *cwd=getcwd(NULL,0);
	fprintf(stdout, "%s [%s] $: ",prefix,cwd);	/* print prompt (printf requires %% to print %) */
	free(cwd);
    fflush(stdout);
    return;
}

void reapChildNoHang(void) {
	temp=head;
	while(temp!=NULL){
		if ((waitpid(temp->data, &status, WNOHANG)) > 0){
			printf("reaped child with pid: %d\n",temp->data);
			delete(temp->data);
			temp = head;
		}
		else{
			temp=temp->next;
		}
	}
}

void changeForegroundProcess(pid_t pgid) {
	int status;
	tcsetpgrp(0,pgid);
	signal(SIGTTOU, SIG_IGN);
	tcsetpgrp(1,pgid);
	signal(SIGTTOU, SIG_IGN);
	tcsetpgrp(2,pgid);
	signal(SIGTTOU, SIG_IGN);
	kill(- pgid,SIGCONT);
	waitpid(pgid,&status,0);
	tcsetpgrp(0, getpid());
	tcsetpgrp(1, getpid());
	tcsetpgrp(2, getpid());
	signal(SIGTTOU, SIG_DFL);
}

int checkForRedirectsAndModifyArgNo(int arg_no, char** arg) {
	if(arg_no >=3){
		if(strcmp(arg[arg_no-2],">")==0){
			printf("detected\n");
			if(!noclobberVal){
				int fileStatus=open(arg[arg_no-1],O_RDWR|O_CREAT, 0666);
				close(1);
				dup(fileStatus);
				close(fileStatus);
				arg_no=arg_no-2;
			}
			else{
				int fileStatus=open(arg[arg_no-1],O_RDWR);
				if(fileStatus==-1){
					int tempfileStatus=open(arg[arg_no-1],O_RDWR|O_CREAT, 0666);
					close(1);
					dup(tempfileStatus);
					close(tempfileStatus);
					arg_no=arg_no-2;
				}
				else{
					printf("File exists, will no overwrite\n");
					exit(0);
				}
			}
		}
		//Makes file breaks redirect
		else if(strcmp(arg[arg_no-2],">&")==0){
			if(!noclobberVal){
				int fileStatus=open(arg[arg_no-1],O_RDWR|O_CREAT, 0666);
				close(1);
				dup(fileStatus);
				close(2);
				dup(fileStatus);
				close(fileStatus);
				arg_no=arg_no-2;
			}
			else{
				int fileStatus=open(arg[arg_no-1],O_RDWR);
				if(fileStatus==-1){
					int tempfileStatus=open(arg[arg_no-1],O_RDWR|O_CREAT, 0666);
					close(1);
					dup(tempfileStatus);
					close(2);
					dup(tempfileStatus);
					close(tempfileStatus);
					arg_no=arg_no-2;
				}
				else{
					printf("File exists, will no overwrite\n");
					exit(0);
				}
			}
		}
		else if(strcmp(arg[arg_no-2],">>")==0){
			if(!noclobberVal){
				int fileStatus=open(arg[arg_no-1],O_RDWR|O_CREAT|O_APPEND, 0666);
				close(1);
				dup(fileStatus);
				close(fileStatus);
				arg_no=arg_no-2;
			}
			else{
				int fileStatus=open(arg[arg_no-1],O_RDWR|O_APPEND, 0666);
				if(fileStatus==-1){
					printf("Will not create a file\n");
					exit(0);
				}
				else{
					close(1);
					dup(fileStatus);
					close(fileStatus);
					arg_no=arg_no-2;
				}
			}
		} 
		else if(strcmp(arg[arg_no-2],">>&")==0){
			if(!noclobberVal){
				int fileStatus=open(arg[arg_no-1],O_RDWR|O_CREAT|O_APPEND, 0666);
				close(1);
				dup(fileStatus);
				close(2);
				dup(fileStatus);
				close(fileStatus);
				arg_no=arg_no-2;
			}
			else{
				int fileStatus=open(arg[arg_no-1],O_RDWR|O_APPEND, 0666);
				if(fileStatus==-1){
					printf("Will not create a file\n");
					exit(0);
				}
				else{
					close(1);
					dup(fileStatus);
					close(2);
					dup(fileStatus);
					close(fileStatus);
					arg_no=arg_no-2;
				}
			}
		}
		else if(strcmp(arg[arg_no-2],"<")==0){
			int fileStatus=open(arg[arg_no-1],O_RDONLY, 0666);
			close(0);
			dup(fileStatus);
			close(fileStatus);
			arg_no=arg_no-2;
		} 
	}
	return arg_no;
}

