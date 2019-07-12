//Parent Process
//This Process call a child process(blackbox call)
//After fork() execute execlp()
//sigHandler()
//signal()

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<wait.h>
#include<sys/time.h>

#define vr_time 30 // video runtime
struct timeval UTCtime_s, UTCtime_e, UTCtime_r;
int sec;

long disp_runtime(struct timeval UTCtime_s, struct timeval UTCtime_e)
{
	struct timeval UTCtime_r;
	if((UTCtime_e.tv_usec- UTCtime_s.tv_usec)<0)
	{
		UTCtime_r.tv_sec  = UTCtime_e.tv_sec - UTCtime_s.tv_sec - 1;
		UTCtime_r.tv_usec = 1000000 - UTCtime_e.tv_usec - UTCtime_s.tv_usec;
	}
	else
	{
		UTCtime_r.tv_sec = UTCtime_e.tv_sec - UTCtime_s.tv_sec;
		UTCtime_r.tv_usec = UTCtime_e.tv_usec - UTCtime_s.tv_usec;
	}
	printf("runtime : %ld \n", UTCtime_r.tv_sec);
	return UTCtime_r.tv_sec;
}

void sigHandler(int sig)
{
	swtich(sig)
	{
		case SIGINT:
					exit(0);
					break;

		case SIGUSR1:
					gettimeofday(&UTCtime_e,NULL);
					if(UTCtime_s.tv_sec != 0)
					{
						long sec = disp_runtime(UTCtime_s,UTCtime_e);
						if(sec > vr_time+2)
						{
							printf("error !! over 35 sec !!\n");
							exit(0);
						}
					}
					else
						printf("black box starting!\n");
					gettimeofday(&UTCtime_s,NULL);

					break;
		default:
					printf("sig number: %d\n",sig);
	}
}

void main(int argc, char* argv[])
{
	pid_t pid;

	int status;	
	int ret;

	signal(SIGUSR1,sigHandler);
	signal(SIGINT,sigHandler);

	pid = fork();
	printf("pid = %d",pid);
	switch(pid)
	{
		case	-1 : perror("fork failed"); exit(0);
		case 	 0 : ret = execlp("./blackbox","blackbox",NULL);
					 fprintf(stderr,"execlp error:%s\n",strerror(errno));
					 exit(0);
		default : 
					 break;
	}
	pid = wait(&status);
	printf("[exit pid] = %d\n",pid);
}

