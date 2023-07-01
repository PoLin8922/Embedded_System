#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char command[25];
    strcpy(command,"echo 1 > /dev/etx_device\0");
    for(int i = 0; i < strlen(argv[1]); i++){
        command[5] = argv[1][i];
        for(int i=0; i<25; i++){
            printf("%c", command[i]);
        }
        system(command);
        printf("\n");
        sleep(1);
    }
}