#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_CLIENTS 10

int balance = 0;  // 帳戶餘額
pthread_mutex_t lock;  // 互斥鎖

// 連線處理函式
void *connection_handler(void *socket_desc) {
    int sock = *(int *)socket_desc;
    char buffer[1024] = {0};
    int valread;

    // 連線建立後，接收並處理客戶端的操作
    while ((valread = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        char operation[20];
        int amount;
        sscanf(buffer, "%s %d", operation, &amount);

        pthread_mutex_lock(&lock);  // 上鎖
        if (strcmp(operation, "deposit") == 0) {
            balance += amount;
            sprintf(buffer, "After deposit: %d", balance);
        } else if (strcmp(operation, "withdraw") == 0) {
            if (balance >= amount) {
                balance -= amount;
                sprintf(buffer, "After withdraw: %d", balance);
            } else {
                sprintf(buffer, "Insufficient balance.");
            }
        } else {
            sprintf(buffer, "Invalid operation.");
        }
        printf("%s\n",buffer);
        pthread_mutex_unlock(&lock);  // 解鎖

        send(sock, buffer, sizeof(buffer), 0);
    }

    if (valread < 0) {
        perror("Error in reading client request");
    }

    close(sock);
    free(socket_desc);
    pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // 建立 socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 設定 socket 屬性
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // 設定 address 結構
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(atoi(argv[1]));

    // 綁定 socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof
    (address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 監聽連線
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Waiting for incoming connections...\n");

    pthread_t thread_id[MAX_CLIENTS];
    int client_count = 0;

    // 初始化互斥鎖
    if (pthread_mutex_init(&lock, NULL) != 0) {
        perror("Mutex initialization failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Connection accepted\n");
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0) {
            perror("accept failed");
            continue;
        }

        // 動態配置記憶體以儲存新連線的 socket 描述符
        int *new_sock = malloc(sizeof(int));
        *new_sock = new_socket;

        // 建立執行緒處理連線
        if (pthread_create(&thread_id[client_count], NULL, connection_handler, (void *)new_sock) < 0) {
            perror("could not create thread");
            free(new_sock);
            continue;
        }

        // 分離執行緒，讓它們可以獨立運行並回收資源
        if (pthread_detach(thread_id[client_count]) < 0) {
            perror("could not detach thread");
            free(new_sock);
            continue;
        }

        client_count++;
        if (client_count >= MAX_CLIENTS) {
            printf("Maximum number of clients reached. No longer accepting connections.\n");
            break;
        }
    }

    // 銷毀互斥鎖
    pthread_mutex_destroy(&lock);

    return 0;
}

