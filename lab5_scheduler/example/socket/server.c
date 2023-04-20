#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 5 // maximun client number
#define BUFFER_SIZE 1024 

int clients[MAX_CLIENTS]; 
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // Mutual exclusion

void *connection_handler(void *socket_desc);

int main(int argc, char *argv[]) {
    int server_socket, client_socket, c, *new_sock;
    struct sockaddr_in server, client;
    char *message;

    // 创建套接字
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        printf("Could not create socket");
        return 1;
    }

    // 设置服务器地址和端口号
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons((u_short)atoi(argv[1]));

    // 绑定套接字到指定地址和端口号
    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind failed. Error");
        return 1;
    }

    // 监听客户端连接请求
    listen(server_socket, MAX_CLIENTS);

    // 等待客户端连接
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client, (socklen_t *)&c))) {
        puts("Connection accepted");

        // 创建新线程处理客户端连接
        pthread_t sniffer_thread;
        new_sock = (int*)malloc(sizeof(int));
        *new_sock = client_socket;

        if (pthread_create(&sniffer_thread, NULL, connection_handler, (void *)new_sock) < 0) {
            perror("could not create thread");
            return 1;
        }

        // 将新客户端的套接字描述符存储到数组中
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

    // 从客户端读取数据
    while ((read_size = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        // 将数据发送给所有客户端
        pthread_mutex_lock(&lock);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] != 0 && clients[i] != sock) {
                write(clients[i], buffer, strlen(buffer));
                printf("output: %s", buffer);
            }
        }
        pthread_mutex_unlock(&lock);

        // 清空缓冲区
        memset(buffer, 0, BUFFER_SIZE);
    }

    // 客户端断开连接
    if (read_size == 0) {
        puts("Client disconnected");

        // 从客户端套接字描述符数组中移除断开连接的客户端
        pthread_mutex_lock(&lock);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] == sock) {
                clients[i] = 0;
                break;
            }
        }
        pthread_mutex_unlock(&lock);
    } else if (read_size == -1) {
        perror("recv failed");
    }

    // 释放套接字描述符
    free(socket_desc);

    return 0;
}

/*
这个程序使用一个 `clients` 数组来存储所有已连接的客户端的套接字描述符，
这个数组最多可以存储 `MAX_CLIENTS` 个客户端。每当一个新客户端连接到服务器时，
服务器将其套接字描述符存储到 `clients` 数组中，并使用一个新线程来处理该客户端的连接请求。
当客户端发送消息时，服务器将消息转发给所有其他已连接的客户端。当一个客户端断开连接时，服务器将其从 `clients` 数组中移除。
在使用共享数据结构 `clients` 数组时，需要使用互斥锁来防止并发访问问题。
*/