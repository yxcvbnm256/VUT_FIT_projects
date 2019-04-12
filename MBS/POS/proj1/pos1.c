#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1 /* XPG 4.2 - needed for WCOREDUMP() */
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

/* function that prints out the status before forking a new child */
void beforeForkInfo(char* label, pid_t pid, pid_t ppid, pid_t pgrp, uid_t uid, uid_t euid, gid_t gid, gid_t egid) 
{
	printf("%s identification: \n", label);
	printf("	pid = %d,	ppid = %d,	pgrp = %d\n", pid, ppid, pgrp);
	printf("	uid = %d,	gid = %d\n", uid, gid);
	printf("	euid = %d,	egid = %d\n", euid, egid);
	return;
}

/* function that prints out the status after a child is ended */
void afterExitInfo(char* label, pid_t pid, int status)
{
	printf("%s exit (pid = %d):", label, pid);
	if (WIFEXITED(status))
	{
		int es = WEXITSTATUS(status);
		printf("	normal termination (exit code = %d)\n", es);
    }
    #ifdef WCOREDUMP
    else if (WCOREDUMP(status))
    {
    	int es = WTERMSIG(status);
    	printf("	signal termination %s(signal = %d)\n","with core dump ", es);
    }
    #endif
    else if (WIFSIGNALED(status))
    {
    	int es = WTERMSIG(status);
    	printf("	signal termination %s(signal = %d)\n","", es);
    }
    else
    {
    	printf("	unknown type of termination\n");
    }
    return;
}

/* ARGSUSED */
int main(int argc, char *argv[])
{
	pid_t grandfatherPID, fatherPID, childPID;
	char* grandparentLabel = "grandparent";
	char* parentLabel = "parent";
	char* childLabel = "child";
	int status = 0;
	// the program needs to have at least one parameter specifying the program that should be executed
	if (argc <= 1)
	{
		printf("You must specify at least one parameter.\n");
		exit(1);
	}
	// process called "grandparent" 
	beforeForkInfo(grandparentLabel, getpid(), getppid(), getpgrp(), getuid(), geteuid(), getgid(), getegid());
	// create child process called "parent"
	fatherPID = fork();
	if (fatherPID == 0)
	{
		//new process called "parent"
		beforeForkInfo(parentLabel, getpid(), getppid(), getpgrp(), getuid(), geteuid(), getgid(), getegid());
		// create child process called "child"
		childPID = fork();
		if (childPID == 0)
		{
			// new process called "child"
			beforeForkInfo(childLabel, getpid(), getppid(), getpgrp(), getuid(), geteuid(), getgid(), getegid());
			int i = 0;
			// align argv array so it shifts one position to the left - because argv[0] is an unwanted executable path 
			for (; i < argc-1; i++)
			{        
				argv[i]=argv[i+1];
			}
			// arguments array needs to be "closed" with NULL pointer in order to execv work properly
			argv[i] = NULL;	
			// "child" process now executes a program specified in the first pointer of argv, accompanied by this program parameters in argv rest
			execv(argv[0], argv); 
			// if error encountered during execution of the specified program, print error
			perror("Error");		
			exit(1);
		}
		else
		{		
			// "parent" waits for the "child" to end	
			while ((fatherPID = wait(&status)) < 0);
			// "child" ended
			afterExitInfo(childLabel, fatherPID, status);			
			exit(0);
		}		
	}
	else
	{
		// "grandparent" waits for the "parent" to end
		while ((grandfatherPID = wait(&status)) < 0);
		// "parent" eded
		afterExitInfo(parentLabel, grandfatherPID, status);
		exit(0);
	}
	return 0;
}
