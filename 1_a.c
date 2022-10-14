#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
int main(int argc, char *argv[]){

	
	pid_t pid=fork(); //The first fork call
	
	if(pid==0){
		//This is the child process
		printf("Child PPID :%d\n Child PID : %d\n",getppid(),getpid());
		
	
	}
	else if(pid<0){
	exit(1);
	}
	else	{ 
		sleep(10);
		system("ps ajx | grep fork");
		
		}
	exit(0);
}
