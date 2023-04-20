#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define MESSAGE_SIZE 1000

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server;
    char message[MESSAGE_SIZE], server_reply[MESSAGE_SIZE];

    // 创建套接字
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket failed");
        return 1;
    }
    puts("Socket created");

    // 设置服务器地址和端口
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons((u_short)atoi(argv[1]));

    // 连接到服务器
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect failed");
        return 1;
    }
    puts("Connected");

    // 从标准输入读取消息并发送给服务器
    while (1) {
        printf("Enter message: ");
        fgets(message, 1000, stdin);
        //printf("Read from stdin: %s", message);

        if (send(sock, message, strlen(message), 0) < 0) {
            perror("send failed");
            return 1;
        }

        // 从服务器接收响应并打印到标准输出
        if (recv(sock, server_reply, MESSAGE_SIZE, 0) < 0) {
            perror("recv failed");
            break;
        }

        printf("Server reply: %s", server_reply);
    }

    close(sock);

    return 0;
}
