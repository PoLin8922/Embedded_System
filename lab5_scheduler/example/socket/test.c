#include <stdio.h>  // perror()
#include <stdlib.h> // exit()
#include <fcntl.h>  // open()
#include <unistd.h> // dup2()

int main() {
    char *args[] = {"sl", "-l", NULL};
    execv("/usr/games/sl", args);

    return 0;
}