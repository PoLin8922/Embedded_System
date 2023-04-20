#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int server_socket, client_socket, c;
    struct sockaddr_in server, client;
    char buffer[BUFFER_SIZE];

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        printf("Could not create socket");
        return 1;
    }

    // Set server address and port
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    // Bind socket to address and port
    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind failed. Error");
        return 1;
    }

    // Listen for client connections
    listen(server_socket, 1);

    // Wait for client connection
    puts("Waiting for incoming connection...");
    c = sizeof(struct sockaddr_in);
    client_socket = accept(server_socket, (struct sockaddr *)&client, (socklen_t *)&c);
    if (client_socket < 0) {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");

    // Redirect stdout to client socket descriptor
    dup2(client_socket, STDOUT_FILENO);

    // Create new process to handle client command
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        // Redirect stdin and stdout to client socket descriptor
        dup2(client_socket, STDIN_FILENO);
        dup2(client_socket, STDOUT_FILENO);

        // Execute "sl -l" command
        char *args[] = {"sl", "-l", NULL};
        execv("/usr/games/sl", args);

        // If execv returns, there was an error
        //perror("execv failed");
        return 1;
    } else if (pid < 0) {
        // Fork failed
        perror("fork failed");
        return 1;
    } else {
        // Parent process
        // Wait for child process to complete
        waitpid(pid, NULL, 0);
    }

    // Close client socket and server socket
    close(client_socket);
    close(server_socket);

    return 0;
}
