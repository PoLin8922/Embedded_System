#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char const *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // 建立 socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    // 設定 server address 結構
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));

    // 轉換 IP address 字串為二進位格式
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // 連線 server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    int amount;
    int times = atoi(argv[5]);
    int count = 0;
    while (count < times)
    {
        if(strstr(argv[3], "deposit") != NULL){
            amount = atoi(argv[4]);
            sprintf(buffer, "%s %d", argv[3], amount);
            send(sock, buffer, sizeof(buffer), 0);
            //printf("count :%d\n",count);

        }
        else if (strstr(argv[3], "withdraw") != NULL){
            amount = atoi(argv[4]);
            sprintf(buffer, "%s %d", argv[3], amount);
            send(sock, buffer, sizeof(buffer), 0);
            //printf("count :%d\n",count);
        }
        

        valread = read(sock, buffer, sizeof(buffer));
        count++;
        if (valread <= 0) {
            printf("Error in reading server response\n");

        }
    }

    // 程式結束前關閉 socket
    close(sock);

    return 0;
}