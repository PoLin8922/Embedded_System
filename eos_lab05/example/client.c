
//#ifndef _SOCKOP_H_
#define _SOCKOP_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> // open()

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#define errexit(format, arg...) exit(printf(format, ##arg))
// #include "sockop.h"
#define BUFSIZE 4096

int connectsock(const char *host, const char *service, const char *transport)
{
	struct hostent *phe; /* pointer to host information entry */
	struct servent *pse; /* pointer to service information entry */
	struct sockaddr_in sin; /* an Internet endpoint address */
	int s, type; /* socket descriptor and socket type */

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;

	/* Map service name to port number */
	if ((pse = getservbyname(service, transport)))
		sin.sin_port = pse->s_port;
	else if ((sin.sin_port = htons((unsigned short)atoi(service))) == 0)
		errexit("Cant_get_\"%s\"_service_entry\n", service);
	/* Use protocol to choose a socket type */
	if (strcmp(transport, "udp") == 0)
		type = SOCK_DGRAM;
	else
		type = SOCK_STREAM;

	/* Allocate a socket */
	s = socket(PF_INET, type, 0);
	if (s < 0)
		errexit("Cant_create_socket:_%s\n", strerror(errno));
	/* Connect the socket */
	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		errexit("Cant_connect_to_%s_%s:_%s\n", host, service,
			strerror(errno));
	return s;
}
int main(int argc, char *argv[])
{
	int connfd; /* socket descriptor */
	int n;
	char buf[BUFSIZE];
	if (argc != 4)
		errexit(" Usage: %s host_address host_port message\n", argv[0]);

	/* create socket and connect to server */
	connfd = connectsock(argv[1], argv[2], "tcp");

	/* write message to server */
	if ((n = write(connfd, argv[3], strlen(argv[3]))) == -1)
		errexit(" Error: write()\n");

	/* read message from the server and print */
	memset(buf, 0, BUFSIZE);

	while (1) {
		if ((n = read(connfd, buf, BUFSIZE)) == -1)
			errexit(" Error: read()\n");
		printf("%s\n", buf);
        sleep(0.1);
	}
	// if ((n = read(connfd, buf, BUFSIZE)) == -1)
	// 	errexit(" Error: read()\n");
	// printf("%s\n", buf);

	/* close client socket */
	close(connfd);
	return 0;
}