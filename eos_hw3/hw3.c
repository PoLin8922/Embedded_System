#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/wait.h>
#include <time.h>
#include <stdbool.h>

#define errexit(format, arg...) exit(printf(format, ##arg))

#define BUFSIZE 1024
#define NUM_THREADS 5
#define MAX_CLIENTS 500
#define STRING_SIZE 10

FILE *eexit;
// void handler(int signum) {
//     while (waitpid(-1, NULL, WNOHANG) > 0);
// }
pthread_mutex_t lock;  // 互斥
void *client_thread(void *socket_desc);
void handler(int signum) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}
void *time_controler(void* arg);

// global variable for strings
char *place[9] = {"Baseball Stadium","Big City","Neiwan Old Street","NYCU","Smangus","17 km of Splendid Coastline","100 Tastes","Science Park","City God Temple"};
char *people[3] = {"Child","Adult","Elder"};
typedef struct {
    int child, adult, elder, total;
} place_info;
place_info station[9]; // store number of people in specific station

// bus info
struct shuttle_info{
    int remain_time;
    int destination[10];
    int total_time[10];
};
struct shuttle_info shuttle1;
struct shuttle_info shuttle2;

void calculate_remain_time(struct shuttle_info *shuttle){
    shuttle->remain_time = 0;
    for(int i=0; i<10; i++){
        shuttle->remain_time += shuttle->total_time[i];
    }
}

// store report people
char *showreport(int place_table[9][3]){
    char buff[1000] = {0};
    char tmpp[10] = {0};
    for(int i = 0; i < 9; i++){
        if(place_table[i][0] || place_table[i][1] || place_table[i][2]){
            strcat(buff,place[i]);
            for(int j = 0; j < 3; j++){
                if(place_table[i][j]){
                    strcat(buff," | ");
                    strcat(buff,people[j]);
                    strcat(buff," ");
                    sprintf(tmpp,"%d",place_table[i][j]);
                    strcat(buff,tmpp);
                }
            }
            strcat(buff,"\n");
        }
    }
    char *return_str = buff;
    return return_str;
}


// schedule two shuttle management function
int schedule_shuttle(int current_time,int dest){
    struct shuttle_info shuttle_info;
    int total_time = current_time;
    /* check merge*/
    int return_time = 0, shuttle = 0;
    int i = 0;
    // shuttle to place
    for(i ; i < 10; i++){
        if(shuttle1.destination[i] == dest){
            shuttle = 1;
            break;
        }
        if(shuttle2.destination[i] == dest){
            shuttle = 2;
            break;
        }
    }

    int merge = return_time;
    // merge shuttle if same destination 
    for(int j = 0; j <= i; j++){
        if(shuttle ==1) return_time += shuttle1.total_time[i];
        else if(shuttle ==2) return_time += shuttle2.total_time[i];
    }

    if(merge && merge > total_time) total_time = merge;
    else if(shuttle1.remain_time <= shuttle2.remain_time){
        if(shuttle1.remain_time + dest > total_time) total_time = shuttle2.remain_time + dest;
        /* insert destination*/
        int k = 0;
        while(shuttle_info.destination[k]){
            k++;
        }
        shuttle_info.destination[k] = dest;
        shuttle_info.total_time[k] = dest;
        /* calculate remain time*/
        shuttle_info.remain_time = 0;
        for(int z = 0; z< 10; z++){
            shuttle_info.remain_time += shuttle_info.total_time[z];
        }   
    } 
    else{
        if(shuttle2.remain_time + dest > total_time) total_time = shuttle2.remain_time + dest;

        /* insert destination*/
        int k = 0;
        while(shuttle_info.destination[k]){
            k++;
        }
        shuttle_info.destination[k] = dest;
        shuttle_info.total_time[k] = dest;
        /* calculate remain time*/
        shuttle_info.remain_time = 0;
        for(int z = 0; z < 10; z++){
            shuttle_info.remain_time += shuttle_info.total_time[z];
        }   
    } 

    return total_time;
}

//  time_controller
void *time_controller(){
    struct shuttle_info shuttle_info;
        while(1){
        if(shuttle1.remain_time > 0) shuttle1.remain_time--;
        if(shuttle2.remain_time > 0) shuttle2.remain_time--;
        if(shuttle1.total_time[0]){
            shuttle1.total_time[0]--;
            if(!shuttle1.total_time[0]){
                for(int i=0; i<9; i++){
                    shuttle_info.destination[i] =  shuttle_info.destination[i+1];
                    shuttle_info.total_time[i] = shuttle_info.total_time[i+1];
                }
                shuttle_info.destination[9] = 0;
                shuttle_info.total_time[9] = 0;
            }
        } 
        if(shuttle2.total_time[0]){
            shuttle2.total_time[0]--;
            if(!shuttle2.total_time[0]){
                for(int i=0; i<9; i++){
                    shuttle_info.destination[i] =  shuttle_info.destination[i+1];
                    shuttle_info.total_time[i] = shuttle_info.total_time[i+1];
                }
                shuttle_info.destination[9] = 0;
                shuttle_info.total_time[9] = 0;
            }
        }
        sleep(1);
    }
}


// search function
void search(char *cmd,int connfd){
    char pushback[BUFSIZE]="";
    if(strlen(cmd) == strlen("Search")){
            sprintf(pushback, "1. Baseball Stadium : %d\n2. Big City : %d\n3. Neiwan Old Street : %d\n4. NYCU : %d\n5. Smangus : %d\n6. 17 km of Splendid Coastline : %d\n7. 100 Tastes : %d\n8. Science Park : %d\n9. City God Temple : %d\n" \
            , station[0].total, station[1].total, station[2].total, station[3].total, station[4].total, station[5].total, station[6].total, station[7].total, station[8].total);
    }
    else if(strlen(cmd) > strlen("Search")){
        if(strstr(cmd,"Baseball Stadium")) sprintf(pushback, "Baseball Stadium - Child : %d | Adult : %d | Elder : %d\n", station[0].child,station[0].adult,station[0].elder);
        if(strstr(cmd,"Big City")) sprintf(pushback, "Big City - Child : %d | Adult : %d | Elder : %d\n", station[1].child,station[1].adult,station[1].elder);
        if(strstr(cmd,"Neiwan Old Street")) sprintf(pushback, "Neiwan Old Street - Child : %d | Adult : %d | Elder : %d\n", station[2].child,station[2].adult,station[2].elder);
        if(strstr(cmd,"NYCU")) sprintf(pushback, "NYCU - Child : %d | Adult : %d | Elder : %d\n", station[3].child,station[3].adult,station[3].elder);
        if(strstr(cmd,"Smangus")) sprintf(pushback, "Smangus - Child : %d | Adult : %d | Elder : %d\n", station[4].child,station[4].adult,station[4].elder);
        if(strstr(cmd,"17 km of Splendid Coastline")) sprintf(pushback, "17 km of Splendid Coastline - Child : %d | Adult : %d | Elder : %d\n", station[5].child,station[5].adult,station[5].elder);
        if(strstr(cmd,"100 Tastes")) sprintf(pushback, "100 Tastes - Child : %d | Adult : %d | Elder : %d\n", station[6].child,station[6].adult,station[6].elder);
        if(strstr(cmd,"Science Park")) sprintf(pushback, "Science Park - Child : %d | Adult : %d | Elder : %d\n", station[7].child,station[7].adult,station[7].elder);
        if(strstr(cmd,"City God Temple")) sprintf(pushback, "City God Temple - Child : %d | Adult : %d | Elder : %d\n", station[8].child,station[8].adult,station[8].elder);
    }
    printf("%s\n",pushback);
        // final pushback report info to client 
    if(send(connfd,pushback, 256,0)<0){
        perror("search function send to client failed\n");
    }
}

// report function
void report(char *cmd,int connfd){
    char pushback[BUFSIZE]="";
    int cmd_len = 0;
    int counter = 0; // count seperation steps = string length
    char *cmd_sep[30];
    // seperate command strings 
    char *token = strtok(cmd, "|");
    while (token != NULL)
    {
        // smd_sep store seperation command 
        cmd_sep[counter++] = token;
        token = strtok(NULL, "|");
    }

    // initialize time and bus scheduler
    int wait_time = 0;
    int place_index = 0;
    int place_table[9][3];
    // clear table
    for(int i = 0;i < 9; i++){
        for(int j = 0; j < 3; j++){
            place_table[i][j] = 0;
        }
    }
    
    // check every element in seperation strings
    /* shuttle need to be scheduling here*/
    for(int i = 0; i < counter; i++){
        // report station human num to client
        if(strstr(cmd_sep[i],place[0])){
            place_index =0;
            wait_time = schedule_shuttle(wait_time,1);
            // wait_time = 1;
        }
        else if(strstr(cmd_sep[i],place[1])){
            place_index =1;
            wait_time = schedule_shuttle(wait_time,2);
            // wait_time = 2;
        }
        else if(strstr(cmd_sep[i],place[2])){
            place_index =2;
            wait_time = schedule_shuttle(wait_time,3);
            // wait_time = 3;
        }
        else if(strstr(cmd_sep[i],place[3])){
            place_index =3;
            // wait_time = 4;
            wait_time = schedule_shuttle(wait_time,4);
        }
        else if(strstr(cmd_sep[i],place[4])){
            place_index =4;
            // wait_time = 5;
            wait_time = schedule_shuttle(wait_time,5);
        }
        else if(strstr(cmd_sep[i],place[5])){
            place_index =5;
            wait_time = schedule_shuttle(wait_time,6);
            // wait_time = 6;
        }
        else if(strstr(cmd_sep[i],place[6])){
            place_index =6;
            wait_time = schedule_shuttle(wait_time,7);
            // wait_time = 7;

        }
        else if(strstr(cmd_sep[i],place[7])){
            place_index =7;
            wait_time = schedule_shuttle(wait_time,8);
            // wait_time = 8;

        }
        else if(strstr(cmd_sep[i],place[8])){
            place_index =8;
            wait_time = schedule_shuttle(wait_time,9);
            // wait_time = 9;
        }
        else{
            // counting total human in station add/delete
            pthread_mutex_lock(&lock);
            char *coin;
            // which people number should be change
            if(strstr(cmd_sep[i]+1,"Ch") != NULL){
                coin = strtok(cmd_sep[i]+1," ");
                coin = strtok(NULL, " "); // NULL = same string using before -> this will give people amount
                station[place_index].child += atoi(coin);
                place_table[place_index][0] += atoi(coin);
                // report_table
            }
            else if(strstr(cmd_sep[i]+1,"Ad") != NULL){
                coin = strtok(cmd_sep[i]+1," ");
                coin = strtok(NULL, " "); 
                station[place_index].adult += atoi(coin);
                place_table[place_index][1] += atoi(coin);

            }
            else if(strstr(cmd_sep[i]+1,"El") != NULL){
                coin = strtok(cmd_sep[i]+1," ");
                coin = strtok(NULL, " "); 
                station[place_index].elder += atoi(coin);
                place_table[place_index][2] += atoi(coin);

            }

            // counting place total people
            station[place_index].total =  station[place_index].child + station[place_index].adult + station[place_index].elder;
            pthread_mutex_unlock(&lock);
        }
    }
    // report to client 
    printf("The shuttle bus is on it's way...\n");
    if (send(connfd,"The shuttle bus is on it's way...\n", 256, 0) < 0) {
        perror("Failed to send response to client");
    } 

    sleep(wait_time);
    // print report table to buffer
    memset(pushback, 0, BUFSIZE);
    sprintf(pushback,"%s",showreport(place_table));
    printf("%s\n",pushback);

    // final pushback report info to client 
    if(send(connfd, pushback, 256, 0)<0){
        perror("report function send to client failed\n");
    }

}

// file initialize
void cleanup()
{
	int child = 0, adult = 0, elder = 0;
	for(int i=0; i<9; i++)
	{
		child += station[i].child;
		adult += station[i].adult;
		elder += station[i].elder;
	}
	fprintf(eexit,"Total : %d\nChild : %d\nAdult  : %d\nElder : %d\n", child+adult+elder, child, adult, elder);
	fclose(eexit);
	exit(-1);
}


int main(int argc, char const *argv[]) {
    /* create txt to store*/
 	eexit = fopen("result.txt", "w");
 	if(eexit == NULL)
    {
        printf("Error!\n");   
        exit(1);             
    }
    signal(SIGINT, cleanup);

    int server_socket, client_socket, c, *new_sock;
    struct sockaddr_in server, client;
    // initialize global variable
    for(int i=0; i<9; i++){
        station[i].child = 0;
        station[i].adult = 0;
        station[i].elder = 0;
        station[i].total = 0;
    }

    shuttle1.remain_time = 0;
    shuttle2.remain_time = 0;
    for (int i = 0; i < 10; i++) {
        shuttle1.destination[i] = 0;
        shuttle2.destination[i] = 0;
        shuttle1.total_time[i] = 0;
        shuttle2.total_time[i] = 0;
    }

    
    // two shuttle bus and time control
    pthread_t sniffer_thread;
    if (pthread_create(&sniffer_thread, NULL, time_controller, NULL) < 0) {
        perror("could not create thread");
        return 1;
    }

    // socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        printf("Could not create socket");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons((u_short)atoi(argv[1]));

    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind failed. Error");
        return 1;
    }

    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Failed to listen for incoming connections");
        exit(1);
    }

    // wait client connect
    c = sizeof(struct sockaddr_in);
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client, (socklen_t *)&c))) {
        printf("Connection accepted\n");
    
        pthread_t sniffer_thread;
        new_sock = (int*)malloc(sizeof(int));
        *new_sock = client_socket;

        if (pthread_create(&sniffer_thread, NULL, client_thread, (void *)new_sock) < 0) {
            perror("could not create thread");
            return 1;
        }
    }

    if (client_socket < 0) {
        perror("accept failed");
        return 1;
    }

    return 0;
}

// client handler
void *client_thread(void *socket_desc){
    int sock = *(int *)socket_desc;
    // 連線建立後，接收並處理客戶端的操作
    while(1){  
        char buffer[BUFSIZE]={0};
        char pushback[BUFSIZE]={0};
        
        // server read from client 
        int svread = read(sock, buffer, BUFSIZE);
        if (svread < 0) {
            perror("Failed to read message from client\n");
        }
        // printf("buffer : %s",buffer);
        else{
            // receive Exit command 
            if(strstr(buffer,"Exit") != NULL){
                break;
            }
            // receive Menu command
            else if(strstr(buffer,"Menu") != NULL){
                sprintf(pushback,"1. Search\n2. Report\n3. Exit\n");
                send(sock, pushback, 256,0);
            }
            // receive Search command
            else if(strstr(buffer,"Search") != NULL){
                search(buffer,sock);
            }
            // receive Report command 
            else if(strstr(buffer,"Report") != NULL){
                report(buffer,sock);
            }
        }

    }
    close(sock);
    signal(SIGCHLD, handler);
    return 0;
}