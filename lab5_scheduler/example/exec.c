/*
* exec.c
*/
#include <unistd.h>
int main (int argc , char *argv[])
{
    execl("/bin/ls", "/bin/ls", "-r", "-t", "-l", (char *) 0);
    return 0 ;
}