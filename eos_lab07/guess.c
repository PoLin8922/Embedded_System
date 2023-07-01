#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>
#include <time.h> 

#define SHMSZ 1024

typedef struct 
{
	int guess;
	char result[8];
} data;

data *info, *shm;

int main(int argc, char *argv[]) 
{
	//shared memory
	int shmid;
	key_t key = atoi(argv[1]);
	if ((shmid = shmget(key, SHMSZ, 0666)) < 0) 
	{
		perror("shmget");
		exit(1);
	}
	if ((shm = shmat(shmid, NULL, 0)) == (data *) -1) 
	{
		perror("shmat");
		exit(1);
	}
	info = shm;
	struct sigaction sa;
	struct timespec req;
	memset(&req, 0, sizeof(struct timespec));
	req.tv_sec = 1;
	req.tv_nsec = 0;
	// start guessing
	srand(time(NULL));
	int pid = atoi(argv[3]);
	int big = atoi(argv[2]), small = 0;
	//int guess = rand() % big + 1;
	int guess = big/2;	

	printf("[game] Guess: %d\n", guess);
	info->guess = guess;
	while(1)
	{
		kill(pid, SIGUSR1);
		nanosleep(&req, &req);
		//printf("result = %c\n", info->result[0]);

		if(info->result[0] == 's')
		{
			small = guess;
		}
		else if(info->result[0] == 'b')
		{
			big = guess;
		}
		else
		{
			//printf("guess success %d!\n", info->guess);
			break;
		}
		guess = (big+small)/2;
		info->guess = guess;
		printf("[game] Guess: %d\n", guess);
	}	
	shmdt(shm);
	return 0;
}
