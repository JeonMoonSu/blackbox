#include<stdio.h>
#include<signal.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>


void main()
{
	pid_t ppid;
	int count = 1;
	ppid = getppid();
	
	while(1)
	{
		if(count % 5 != 0)
		{
			printf("Hi\n");
			kill(ppid,SIGUSR1);
			count++;
			sleep(1);
		}
		else
		{
			printf("Hi\n");
			kill(ppid,SIGUSR1);
			count++;
			sleep(5);
		}
	}
}
