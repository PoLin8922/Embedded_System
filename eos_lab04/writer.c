#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char command[100];

    strcat(command,"echo ");
    strcat(command, argv[1]);
    strcat(command, " > /dev/my_dev");

    system(command);
    return 0;
}