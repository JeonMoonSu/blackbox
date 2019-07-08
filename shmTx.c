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
	void *shared_Mem = (void*)0;
	int shmid;
	int *shmaddr;
	int i;
	int len;
	FILE* fp;

	if(fp = fopen("./lena.jpg","rb"))
	{
		fseek(fp,0,SEEK_END);
		len = ftell(fp);
		printf("len = %d\n",len);
	}
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

	// Wait until Receive Process is ready
	while(1)
	{
		//start[t]
		if(shmaddr[0] == 't')
		{
			break;
		}
	}
	printf("start!!");

	fseek(fp,0,SEEK_SET);
	while(!feof(fp))
	{
		//start[t] or receive attached[a] message
		if(shmaddr[0] == 'a' || shmaddr[0] == 't')
		{
			int size = fread(shmaddr+1,1,SHMSIZE-1,fp);
			//Last send[l]
			if(size < SHMSIZE-1)
			{
				shmaddr[0] = 'l';
				shmaddr[1] = size;
			}
			else
				//complete data send[s]
				shmaddr[0] = 's';
		}
	}
	//상대방이 마지막 데이터 받을때까지[a] 대기
	while(1)
	{
		if(shmaddr[0] == 'a')
			break;
	}
	//종료선언[e]
	shmaddr[0] = 'e';

	printf("succeed file trasfer\n");

	// step4. shmdt
	if (shmdt(shared_Mem) == -1)
	{
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}
	/*
	// step5. shmctl : IPC_RMID
	if (shmctl(shmid, IPC_RMID, 0) == -1)
	{
	fprintf(stderr, "shmctl (IPC_RMID) failed\n");
	exit(EXIT_FAILURE);
	}
	 */
	exit(EXIT_SUCCESS);
}
