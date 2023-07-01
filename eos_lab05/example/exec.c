#include <unistd.h>

int main(int argc, char *argv[]) {
    // execl("/sl", "/sl", "-l", NULL);
    
    // //execlp("ls", "ls", "-l", NULL);
    
    // char *arg1[] = {"/sl", "-l", NULL};
    // execv(arg1[0], arg1);
    
    // //char *arg2[] = {"ls", "-l", NULL};
    // //execvp(arg2[0], arg2);
    //execl("/sl", "/sl", "-l", NULL);

    char *args[] = {"sl", "-l", NULL};
    execv("/usr/games/sl", args);
    // dup2(sock, STDOUT_FILENO);
    //     char *args[] = {"sl", "-l", NULL};
    //     execv("/usr/games/sl", args);
}