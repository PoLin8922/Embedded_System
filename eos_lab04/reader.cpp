#include <cstdio>      // fprintf(), perror()
#include <cstdlib>     // exit()
#include <cstring>     // memset()
#include <csignal>    // signal()
#include <fcntl.h>     // open()
#include <unistd.h>    // read(), write(), close()

#include <sys/socket.h> // socket(), connect()
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h>  // htons()

int connfd, fd;

char seg_for_c[27][17] = {
    "1111001100010001", // A
    "0000011100000101", // b
    "1100111100000000", // C
    "0000011001000101", // d
    "1000011100000001", // E
    "1000001100000001", // F
    "1001111100010000", // G
    "0011001100010001", // H
    "1100110001000100", // I
    "1100010001000100", // J
    "0000000001101100", // K
    "0000111100000000", // L
    "0011001110100000", // M
    "0011001110001000", // N
    "1111111100000000", // O
    "1000001101000001", // P
    "0111000001010000", //q
    "1110001100011001", //R
    "1101110100010001", //S
    "1100000001000100", //T
    "0011111100000000", //U
    "0000001100100010", //V
    "0011001100001010", //W
    "0000000010101010", //X
    "0000000010100100", //Y
    "1100110000100010", //Z
    "0000000000000000"
};

void sigint_handler(int signo) {
    close(fd);
    close(connfd);
}

int main(int argc, char *argv[]) {
    if(argc != 4) {
        fprintf(stderr, "Usage: ./reader <server_ip> <port> <device_path>");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, sigint_handler);

    if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in cli_addr;
    memset(&cli_addr, 0, sizeof(cli_addr));
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = inet_addr(argv[1]);
    cli_addr.sin_port = htons((u_short)atoi(argv[2]));

    if(connect(connfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) == -1) {
        perror("connect()");
        exit(EXIT_FAILURE);
    }

    if((fd = open(argv[3], O_RDWR)) < 0) {
        perror(argv[3]);
        exit(EXIT_FAILURE);
    }

    int ret;
    char buff[1] = {};

    while (true) {
        char *buf;

        if((ret = read(fd, buff, sizeof(buff))) == -1) {
            perror("read()");
            exit(EXIT_FAILURE);
        }

   

        if(buff[0] >= 'A' && buff[0] <= 'Z'){
            buf =  seg_for_c[buff[0] - 'A'];
        }
        else if(buff[0] >= 'a' && buff[0] <= 'z'){
            buf =  seg_for_c[buff[0] - 'a'];
        }
        else{
            break;
        }


        // for (int i = 0; i < 16; ++i) {
        //     printf("%c ", buf[i]);
        // }
        // printf("\n");

        if(write(connfd, buf, 16) == -1) {
            perror("write()");
            exit(EXIT_FAILURE);
        }



        sleep(1);
    }

    close(fd);
    close(connfd);
    return 0;
}
