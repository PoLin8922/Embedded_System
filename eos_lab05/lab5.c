#define _SOCKOP_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> // open()
#include <pthread.h>

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>   // signal()

// #include "sockop.h"

#define errexit(format, arg...) exit(printf(format, ##arg))

#define BUFSIZE 1024
#define NUM_THREADS 5

void* handle_client(void* arg);
void handle_zombie(int sig);

int passivesock(const char *service, const char *transport, int qlen)
{
	struct servent *pse; /* pointer to service information entry */
	struct sockaddr_in sin; /* an Internet endpoint address */
	int s, type; /* socket descriptor and socket type */

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;

	/* Map service name to port number */
	if ((pse = getservbyname(service, transport)))
		sin.sin_port = htons(ntohs((unsigned short)pse->s_port));
	else if ((sin.sin_port = htons((unsigned short)atoi(service))) == 0)
		errexit("Can't_find_ \"%s\"_service_centry\n", service);

	/* Use protocol to choose a socket type */
	if (strcmp(transport, "udp") == 0)
		type = SOCK_DGRAM;
	else
		type = SOCK_STREAM;

	/* Allocate a socket */
	s = socket(PF_INET, type, 0);
	if (s < 0)
		errexit("Cant_create_socket:_%s\n", strerror(errno));

	/* Bind the socket */
	if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		errexit("Cant_bind_to_port Zs: %s \n", strerror(errno));

	/* Set the maximum number of waiting connection */
	if (type == SOCK_STREAM && listen(s, qlen) < 0)
		errexit("Cant_listen_on_port Zs: %s\n",
			strerror(errno));

	return s;
}


int main(int argc, char *argv[])
{
	int sockfd, connfd; /* socket descriptor */
    int pid;
	struct sockaddr_in addr_cln;
	socklen_t sLen = sizeof(addr_cln);
	int n;
	char snd[BUFSIZE], rev[BUFSIZE];

	/*check input arg*/
	if (argc != 2)
		errexit(" Usage:_%s_port \n", argv[0]);



	/* create socket and bind socket to port */
	sockfd = passivesock(argv[1], "tcp", 10);
	while (1) {
        // accept a new client connection
        connfd = accept(sockfd, (struct sockaddr *) &addr_cln, &sLen);
        if (connfd < 0) {
            perror("accept error");
            continue;
        }

        // fork a new process to handle this client
        pid = fork();
        if (pid < 0) {
            perror("fork error");
            close(connfd);
            continue;
        }

        if (pid == 0) {
            // child process
            close(sockfd); // close the original listening socket in the child process
            // execute the "sl -l" command using execlp() and redirect output to the client socket
            dup2(connfd, STDOUT_FILENO);
            execlp("sl", "sl", "-l", NULL);

            // execlp() only returns if there is an error
            perror("execlp error");
            exit(1);
        } else {
            // parent process
            printf("Train ID = %d\n", pid);
            close(connfd); // close the client socket in the parent process
        }
		signal(SIGCHLD, handle_zombie);

	}
	/* close server socket */
	//close(sockfd);

	return 0;
}

void* handle_client(void* arg) {
    int connfd = (int)(long)arg;

    // deal with client request
    char buffer[1024];
    int n = read(connfd, buffer, sizeof(buffer));
    if (n < 0) {
        perror("read");
        close(connfd);
        return NULL;
    }

    // signal to client
    if (write(connfd, buffer, n) < 0) {
        perror("write");
        close(connfd);
        return NULL;
    }

    close(connfd);

    return NULL;
}

void handle_zombie(int sig) {
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("Child process %d terminated\n", pid);
    }
}