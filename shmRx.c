#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//For IPC 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMSIZE	100 //int * 100

int main(void)
{
	FILE* fp;
	void *shared_Mem = (void*)0;
	int shmid;
	int *shmaddr;
	int rdData;
	int i;

	fp = fopen("./lenacopy.jpg","wb");
	// step0. ftok()

	// step1. shmget
	shmid = shmget((key_t)1234, sizeof(char)*SHMSIZE, 0666 | IPC_CREAT);
	if (shmid == -1)
	{
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
	}

	// step2. shmat
	shared_Mem = shmat(shmid, (void*)0, 0);
	if (shared_Mem == (void*)-1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}

	printf("Memory attached at %X\n", (int)shared_Mem);
	shmaddr = (int*)shared_Mem;

	// step3. memory access
	shmaddr[0] = 't';
	
	while(shmaddr[0] != 'e')
	{
		if(shmaddr[0] == 's')
		{
			fwrite(shmaddr+1,SHMSIZE-1,1,fp);
			shmaddr[0] = 'a';
		}
	}

	fseek(fp,0,SEEK_END);
	int len = ftell(fp);
	printf("sended len = %d\n",len);
	//sleep(4);

	// step4. shmdt
	if (shmdt(shared_Mem) == -1)
	{
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}

	// step5. shmctl : IPC_RMID
	if (shmctl(shmid, IPC_RMID, 0) == -1)
	{
		fprintf(stderr, "shmctl (IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
