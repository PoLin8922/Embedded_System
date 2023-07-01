#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/shm.h>

#define SHMSZ 1024

typedef struct 
{
	int guess;
	char result[8];
} data;

int shmid;
data *info, *shm;
int ans;

void handler (int signal_number)
{
	printf("[game] Guess: %d, ", info->guess);
	if(ans > info->guess)
	{
		printf("bigger\n");
		info->result[0] = 's';
	}
	else if(ans < info->guess)
	{
		printf("smaller\n");
		info->result[0] = 'b';
	}
	else
	{
		printf("bingo\n");
		info->result[0] = 'c';
	}

}

void cleanup() // check ipcs -m with ctrl+c
{
	if (shmid != -1)
	{
		shmctl(shmid, IPC_RMID, NULL);
	}
	exit(-1);
}

int main(int argc, char *argv[]) 
{	
	key_t key = atoi(argv[1]);
	// create shared memory
	if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) 
	{
		perror("shmget");
		exit(1);
	}
	// active access shared memory
	if ((shm = shmat(shmid, NULL, 0)) == (data *) -1) 
	{
		perror("shmat");
		exit(1);
	}
	// info is the shared memory storage place
	info = shm;
	
	ans = atoi(argv[2]);

	struct sigaction sa;
	struct timespec req;
	int retval = 1;
	int pid = getpid();
	/* set the sleep time to 10 sec */
	memset(&req, 0, sizeof(struct timespec));
	req.tv_sec = 10;
	req.tv_nsec = 0;
	/* register handler to SIGUSR1 */
	memset(&sa, 0, sizeof (sa));
	sa.sa_handler = &handler;
	sigaction (SIGUSR1, &sa, NULL);
	signal(SIGINT, cleanup);

	printf("[game] Game pid: %d\n", pid);
	/* sleep 10 sec */
	while(1)
	{
		nanosleep(&req, &req);
	}

	return 0;
}
