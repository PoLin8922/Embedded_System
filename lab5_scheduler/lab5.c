//server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/wait.h>

#define MAX_CLIENTS 5 // maximun client number
#define BUFFER_SIZE 1024 

int clients[MAX_CLIENTS]; 
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // Mutual exclusion

void *connection_handler(void *socket_desc);

void handler(int signum) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[]) {
    int server_socket, client_socket, c, *new_sock;
    struct sockaddr_in server, client;
    char *message;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        printf("Could not create socket");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons((u_short)atoi(argv[1]));

    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind failed. Error");
        return 1;
    }

    listen(server_socket, MAX_CLIENTS);

    // wait client connect
    // printf("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client, (socklen_t *)&c))) {
        // printf("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = (int*)malloc(sizeof(int));
        *new_sock = client_socket;

        if (pthread_create(&sniffer_thread, NULL, connection_handler, (void *)new_sock) < 0) {
            perror("could not create thread");
            return 1;
        }

        pthread_mutex_lock(&lock);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] == 0) {
                clients[i] = client_socket;
                break;
            }
        }
        pthread_mutex_unlock(&lock);
    }

    if (client_socket < 0) {
        perror("accept failed");
        return 1;
    }

    return 0;
}

void *connection_handler(void *socket_desc) {
    int sock = *(int *)socket_desc;
    int read_size;
    char buffer[BUFFER_SIZE] = {0};
    char *args[] = {"sl", "-l", NULL};
    pid_t pid;

    // create child process to execute command
    pid = fork();
    
    if (pid >= 0) { /* fork succeeded */
        if (pid == 0) {
            dup2(sock, STDOUT_FILENO);
            execlp("/usr/games/sl", "sl", "-l", NULL);
        } 
        else { 
            printf("Train ID = %d\n", pid);
        }
    } 
    else { 
        perror("fork error");
        exit(0);
    }

    // wait for child process to finish
    waitpid(pid, NULL, 0);

    // close client socket
    close(sock);
    signal(SIGCHLD, handler);

    return 0;
}