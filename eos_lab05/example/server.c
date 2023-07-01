//#ifndef _SOCKOP_H_
#define _SOCKOP_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>  // open()

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
// #include "sockop.h"
#define errexit(format, arg...) exit(printf(format, ##arg))

#define BUFSIZE 1024

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
		errexit("Cant_bind_to_port Zs: %s\n", service, strerror(errno));

	/* Set the maximum number of waiting connection */
	if (type == SOCK_STREAM && listen(s, qlen) < 0)
		errexit("Cant_listen_on_port Zs: %s\n", service,
			strerror(errno));
	return s;
}


int main(int argc, char *argv[])
{
	int sockfd, connfd; /* socket descriptor */
	struct sockaddr_in addr_cln;
	socklen_t sLen = sizeof(addr_cln);
	int n;
	char snd[BUFSIZE], rev[BUFSIZE];

	if (argc != 2)
		errexit(" Usage:_%s_port \n", argv[0]);

	/* create socket and bind socket to port */
	sockfd = passivesock(argv[1], "tcp", 10);
	while (1) {
		/* waiting for connection */
		connfd = accept(sockfd, (struct sockaddr *)&addr_cln, &sLen);
		if (connfd == -1)
			errexit(" Error: accept()\n");

		/* read message from client */
		if ((n = read(connfd, rev, BUFSIZE)) == -1)
			errexit(" Error: read()\n");

		/* write message back to the client */
		n = sprintf(snd, "Server: %.*s", n, rev);
		if ((n = write(connfd, snd, n)) == -1)
			errexit(" Error: write()\n");
            
		dup2(connfd, STDOUT_FILENO);
		execlp("/usr/games/sl", "sl", "-l", NULL);

		/* close client connection */
		close(connfd);
	}
	/* close server socket */
	close(sockfd);

	return 0;
}
