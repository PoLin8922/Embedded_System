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
#include <signal.h>

#define errexit(format, arg...) exit(printf(format, ##arg))

#define BUFSIZE 1024
#define NUM_THREADS 5

void deleteLeading(char *str)
{
	if(str == NULL || *str == '\0')
	{
		return;
	}
	while(*str != '\0')
	{
		*str = *(str+1);
		str++;
	}
}

int passivesock(const char *service, const char *transport, int qlen)
{
	struct servent *pse;	/* pointer to service information entry */
	struct sockaddr_in sin; /* an Internet endpoint address */
	int s, type;			/* socket descriptor and socket type */

	memset(&sin, 0, sizeof(sin)+1);
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
		errexit("Cant_listen_on_port Zs: %s\n",strerror(errno));

	if (listen(s, 1) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	return s;
}

#define STRING_SIZE 10

struct place_info
{
	int child, adult, elder;
};

int main(int argc, char *argv[])
{


	/* socket connection*/

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

	while (1)
	{
		struct place_info Baseball_Stadium = {.child = 0, .adult = 0, .elder = 0};
		struct place_info Big_City = {.child = 0, .adult = 0, .elder = 0};
		struct place_info Neiwan_Old_Street = {.child = 0, .adult = 0, .elder = 0};
		struct place_info NYCU = {.child = 0, .adult = 0, .elder = 0};
		struct place_info Smangus = {.child = 0, .adult = 0, .elder = 0};
		struct place_info a17_KM_of_Splendid_Coastline = {.child = 0, .adult = 0, .elder = 0};
		struct place_info a100_Tastes = {.child = 0, .adult = 0, .elder = 0};
		struct place_info Science_Park = {.child = 0, .adult = 0, .elder = 0};
		struct place_info City_God_Temple = {.child = 0, .adult = 0, .elder = 0};
		char *token;
		size_t bufsize = 0;
		int wait = 0;
		char *opt, *buff;
		buff = (char *)malloc((sizeof(char) * BUFSIZE));
		opt = (char *)malloc((sizeof(char) * BUFSIZE));

		/* Socket*/
		// accept a new client connection
		// connfd is my clinet
		connfd = accept(sockfd, (struct sockaddr *)&addr_cln, &sLen);
		if (connfd < 0)
		{
			perror("accept error");
			continue;
		}

		/* Start HW2 Menu*/
		//read(connfd, buff, BUFSIZE);

		while (1)
		{
			char cmd[50][30];
			int cmd_len = 0;
			int counter = 0;
			memset(opt, 0, BUFSIZE);
			/*Seperation of Input String*/
			read(connfd, opt, BUFSIZE);
			//printf("client cmd:%s\n",opt);
			token = strtok(opt, "|");
			while (token != NULL)
			{
				strcpy(cmd[cmd_len++], token);
				counter++;
				token = strtok(NULL, "|");
			}

			/* clear socket buffer */
			memset(buff, 0, sizeof(buff));
			memset(opt, 0, BUFSIZE);
			/* Menu */
			if (strstr(cmd[0], "Menu") != NULL)
			{
				sprintf(buff, "1. Search\n2. Report\n3. Exit\n");
				write(connfd, buff, 256);
			}
			/* Search */
			else if (strstr(cmd[0], "Search") != NULL)
			{
				if (counter == 1)
				{
					sprintf(opt, "1. Baseball Stadium : %d\n", Baseball_Stadium.child + Baseball_Stadium.adult + Baseball_Stadium.elder);
					strcat(buff, opt);
					sprintf(opt, "2. Big City : %d\n", Big_City.child + Big_City.adult + Big_City.elder);
					strcat(buff, opt);
					sprintf(opt, "3. Neiwan Old Street : %d\n", Neiwan_Old_Street.child + Neiwan_Old_Street.adult + Neiwan_Old_Street.elder);
					strcat(buff, opt);
					sprintf(opt, "4. NYCU : %d\n", NYCU.child + NYCU.adult + NYCU.elder);
					strcat(buff, opt);
					sprintf(opt, "5. Smangus : %d\n", Smangus.child + Smangus.adult + Smangus.elder);
					strcat(buff, opt);
					sprintf(opt, "6. 17 km of Splendid Coastline : %d\n", a17_KM_of_Splendid_Coastline.child + a17_KM_of_Splendid_Coastline.adult + a17_KM_of_Splendid_Coastline.elder);
					strcat(buff, opt);
					sprintf(opt, "7. 100 Tastes : %d\n", a100_Tastes.child + a100_Tastes.adult + a100_Tastes.elder);
					strcat(buff, opt);
					sprintf(opt, "8. Science Park : %d\n", Science_Park.child + Science_Park.adult + Science_Park.elder);
					strcat(buff, opt);
					sprintf(opt, "9. City God Temple : %d\n", City_God_Temple.child + City_God_Temple.adult + City_God_Temple.elder);
					strcat(buff, opt);
					write(connfd, buff, 256);
				}
				else if (counter >= 2)
				{
					if (strstr(cmd[1], "Baseball") != NULL)
					{
						sprintf(buff, "Baseball Stadium - Child : %d Adult : %d Elder : %d\n", Baseball_Stadium.child, Baseball_Stadium.adult, Baseball_Stadium.elder);
						write(connfd, buff, 256);
					}
					else if (strstr(cmd[1], "Big") != NULL)
					{
						sprintf(buff, "Big City - Child : %d Adult : %d Elder : %d\n", Big_City.child, Big_City.adult, Big_City.elder);
						write(connfd, buff, 256);
					}
					else if (strstr(cmd[1], "Neiwan Old Street") != NULL)
					{
						sprintf(buff, "Neiwan Old Street - Child : %d Adult : %d Elder : %d\n", Neiwan_Old_Street.child, Neiwan_Old_Street.adult, Neiwan_Old_Street.elder);
						write(connfd, buff, 256);
					}
					else if (strstr(cmd[1], "NYCU") != NULL)
					{
						sprintf(buff, "NYCU - Child : %d Adult : %d Elder : %d\n", NYCU.child, NYCU.adult, NYCU.elder);
						write(connfd, buff, 256);
					}
					else if (strstr(cmd[1], "Smangus") != NULL)
					{
						sprintf(buff, "Smangus - Child : %d Adult : %d Elder : %d\n", Smangus.child, Smangus.adult, Smangus.elder);
						write(connfd, buff, 256);
					}
					else if (strstr(cmd[1], "17 km of Splendid Coastline") != NULL)
					{
						sprintf(buff, "17 km of Splendid Coastline - Child : %d Adult : %d Elder : %d\n", a17_KM_of_Splendid_Coastline.child, a17_KM_of_Splendid_Coastline.adult,a17_KM_of_Splendid_Coastline.elder);
						write(connfd, buff, 256);
					}
					else if (strstr(cmd[1], "100 Tastes") != NULL)
					{
						sprintf(buff, "100 Tastes - Child : %d Adult : %d Elder : %d\n", a100_Tastes.child, a100_Tastes.adult, a100_Tastes.elder);
						write(connfd, buff, 256);
					}
					else if (strstr(cmd[1], "Science Park") !=  NULL)
					{
						sprintf(buff, "Science Park - Child : %d Adult : %d Elder : %d\n", Science_Park.child, Science_Park.adult, Science_Park.elder);
						write(connfd, buff, 256);
					}
					else if (strstr(cmd[1], "City God Temple") !=  NULL)
					{
						sprintf(buff, "City God Temple - Child : %d Adult : %d Elder : %d\n", City_God_Temple.child, City_God_Temple.adult, City_God_Temple.elder);
						write(connfd, buff, 256);
					}
				}
			}
			/* Report */
			else if (strstr(cmd[0], "Report") != NULL)
			{
				wait = 0;
				for (int i = 1; i < counter; i += 2)
				{
					if(cmd[i+1][strlen(cmd[i+1])-1]==' ') cmd[i+1][strlen(cmd[i+1])-1]='\0';
					deleteLeading(cmd[i]);
					sprintf(buff, "%s|%s\n", cmd[i], cmd[i+1]);
					if (strstr(cmd[i], "Baseball") != NULL)
					{
						token = strtok(cmd[i + 1], " ");
						token = strtok(NULL, " ");
						if (strstr(cmd[i + 1], "Adult"))
						{
							Baseball_Stadium.adult = Baseball_Stadium.adult + atoi(token);
						}
						else if (strstr(cmd[i + 1], "Elder"))
						{
							Baseball_Stadium.elder = Baseball_Stadium.elder + atoi(token);
						}
						else if (strstr(cmd[i + 1], "Child"))
						{
							Baseball_Stadium.child = Baseball_Stadium.child + atoi(token);
						}
						if (wait < 1)
							wait = 1;
					}
					else if (strstr(cmd[i], "Big") != NULL)
					{
						token = strtok(cmd[i + 1], " ");
						token = strtok(NULL, " ");
						if (strstr(cmd[i + 1], "Adult"))
						{
							Big_City.adult = Big_City.adult + atoi(token);
						}
						else if (strstr(cmd[i + 1], "Elder"))
						{
							Big_City.elder = Big_City.elder + atoi(token);
						}
						else if (strstr(cmd[i + 1], "Child"))
						{
							Big_City.child = Big_City.child + atoi(token);
						}
						if (wait < 2)
							wait = 2;
					}
					else if (strstr(cmd[i], "Neiwan Old Street") != NULL)
					{
						token = strtok(cmd[i + 1], " ");
						token = strtok(NULL, " ");
						if (strstr(cmd[i + 1], "Adult"))
						{
							Neiwan_Old_Street.adult = Neiwan_Old_Street.adult + atoi(token);
						}
						else if (strstr(cmd[i + 1], "Elder"))
						{
							Neiwan_Old_Street.elder = Neiwan_Old_Street.elder + atoi(token);
						}
						else if (strstr(cmd[i + 1], "Child"))
						{
							Neiwan_Old_Street.child = Neiwan_Old_Street.child + atoi(token);
						}
						if (wait < 3)
							wait = 3;
					}
					else if (strstr(cmd[i], "NYCU") != NULL)
					{
						token = strtok(cmd[i + 1], " ");
						token = strtok(NULL, " ");
						if (strstr(cmd[i + 1], "Adult"))
						{
							NYCU.adult = NYCU.adult + atoi(token);
						}
						else if (strstr(cmd[i + 1], "Elder"))
						{
							NYCU.elder = NYCU.elder + atoi(token);
						}
						else if (strstr(cmd[i + 1], "Child"))
						{
							NYCU.child = NYCU.child + atoi(token);
						}
						if (wait < 4)
							wait = 4;
					}
					else if (strstr(cmd[i], "Smangus") != NULL)
					{
						token = strtok(cmd[i + 1], " ");
						token = strtok(NULL, " ");
						if (strstr(cmd[i + 1], "Adult"))
						{
							Smangus.adult = Smangus.adult + atoi(token);
						}
						else if (strstr(cmd[i + 1], "Elder"))
						{
							Smangus.elder = Smangus.elder + atoi(token);
						}
						else if (strstr(cmd[i + 1], "Child"))
						{
							Smangus.child = Smangus.child + atoi(token);
						}
						if (wait < 5)
							wait = 5;
					}
					else if (strstr(cmd[i], "17 km of Splendid Coastline") != NULL)
					{
						token = strtok(cmd[i + 1], " ");
						token = strtok(NULL, " ");
						if (strstr(cmd[i + 1], "Adult"))
						{
							a17_KM_of_Splendid_Coastline.adult = a17_KM_of_Splendid_Coastline.adult + atoi(token);
						}
						else if (strstr(cmd[i + 1], "Elder"))
						{
							a17_KM_of_Splendid_Coastline.elder = a17_KM_of_Splendid_Coastline.elder + atoi(token);
						}
						else if (strstr(cmd[i + 1], "Child"))
						{
							a17_KM_of_Splendid_Coastline.child = a17_KM_of_Splendid_Coastline.child + atoi(token);
						}
						if (wait < 6)
							wait = 6;
					}
					else if (strstr(cmd[i], "100 Tastes") != NULL)
					{
						token = strtok(cmd[i + 1], " ");
						token = strtok(NULL, " ");
						if (strstr(cmd[i + 1], "Adult"))
						{
							a100_Tastes.adult = a100_Tastes.adult + atoi(token);
						}
						else if (strstr(cmd[i + 1], "Elder"))
						{
							a100_Tastes.elder = a100_Tastes.elder + atoi(token);
						}
						else if (strstr(cmd[i + 1], "Child"))
						{
							a100_Tastes.child = a100_Tastes.child + atoi(token);
						}
						if (wait < 7)
							wait = 7;
					}
					else if (strstr(cmd[i], "Science Park") != NULL)
					{
						token = strtok(cmd[i + 1], " ");
						token = strtok(NULL, " ");
						if (strstr(cmd[i + 1], "Adult"))
						{
							Science_Park.adult = Science_Park.adult + atoi(token);
						}
						else if (strstr(cmd[i + 1], "Elder"))
						{
							Science_Park.elder = Science_Park.elder + atoi(token);
						}
						else if (strstr(cmd[i + 1], "Child"))
						{
							Science_Park.child = Science_Park.child + atoi(token);
						}
						if (wait < 8)
							wait = 8;
					}
					else if (strstr(cmd[i], "City God Temple") != NULL)
					{
						token = strtok(cmd[i + 1], " ");
						token = strtok(NULL, " ");
						if (strstr(cmd[i + 1], "Adult"))
						{
							City_God_Temple.adult = City_God_Temple.adult + atoi(token);
						}
						else if (strstr(cmd[i + 1], "Elder"))
						{
							City_God_Temple.elder = City_God_Temple.elder + atoi(token);
						}
						else if (strstr(cmd[i + 1], "Child"))
						{
							City_God_Temple.child = City_God_Temple.child + atoi(token);
						}
						if (wait < 9)
							wait = 9;
					}
					strcat(opt, buff);
				}
				memset(buff, 0, 256);
				/*Before sending message to client ,sleep*/
				sprintf(buff, "Please wait a few seconds...\n");
				write(connfd, buff, 256);
				sleep(wait);
				
				// return information
				write(connfd, opt, 256);				
			}
			/* Exit */
			else if (strstr(cmd[0], "Exit") != NULL)
			{
				close(connfd);
				break;
			}
		}

	}
	return 0;
}

