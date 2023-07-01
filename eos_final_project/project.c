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
#include <limits.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>

#define MAX_CLIENTS 100 // maximun client number
#define BUFFER_SIZE 1024 

int pid, bt[3];
char *BTN_PATH[3] = { "/dev/button_00", "/dev/button_01", "/dev/button_02"};

int led_state[12][4];

int clients[MAX_CLIENTS]; 
unsigned char btn_state[3];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // Mutual exclusion

void *connection_handler(void *socket_desc);
void handler(int signum) {
	while (waitpid(-1, NULL, WNOHANG) > 0);
}

char *delivery_staff[5] = {"Dino", "Jerry", "Andrew", "Ben", "Mark"};
char *shop_name[12] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12"};
int shop_address[2][12] = {{ /*0*/ 3, /*1*/ 2, /*2*/ 5, /*3*/ 5, /*4*/ -4, /*5*/ -1, /*6*/ -3, /*7*/ -5, /*8*/ -5, /*9*/ -1, /*10*/ 2, /*11*/ -3 }, 
	{ /*0*/ 4, /*1*/ 0, /*2*/ -1, /*3*/ -4, /*4*/ 6, /*5*/ 4, /*6*/ 3, /*7*/ 2, /*8*/ -1, /*9*/ -1, /*10*/ -4, /*11*/ -6 }};

struct delivery_man_info{
	int current_position[2];
	int destination[2][20];
	int cost_time[20];
}staff[5];
typedef struct
{
	int value;
} ThreadArgs;

void insert_destination(struct delivery_man_info *delivery_man, int x, int y, int cost){
	int i = 0;
	while(delivery_man -> cost_time[i]){
		i++;
	}

	delivery_man -> destination[0][i] = x;
	delivery_man -> destination[1][i] = y;
	delivery_man -> cost_time[i] = cost;


}

void insert_into_staff_list(struct delivery_man_info *delivery_man, int report_list[12], int client_address[2], int (*state)[50]){
	int nearest_place = 0;

	int staff_position[2];
	staff_position[0] = delivery_man -> current_position[0];
	staff_position[1] = delivery_man -> current_position[1];

	int time = 0, i = 0;
	while(i<20){
		if(delivery_man -> cost_time[i]){
			time += delivery_man -> cost_time[i];
			staff_position[0] = delivery_man -> destination[0][i];
			staff_position[1] = delivery_man -> destination[1][i];
			i++;
		}
		else break;
	}
	state[0][0] = time;



	int min = INT_MAX;
	int n = 1;
	for(int j=0; j<12; j++)
	{
		min = INT_MAX;
		bool check = false;
		for(int k=0; k<12; k++)
		{
			if(report_list[k])
			{
				check = true;
				int tmp =  abs(shop_address[0][k] - staff_position[0]) + abs(shop_address[1][k] - staff_position[1]);
				if(tmp < min)
				{
					min = tmp;
					nearest_place = k;
				}
			}
		}
		if(!check)
			break;
		else{
			insert_destination(delivery_man, shop_address[0][nearest_place], shop_address[1][nearest_place], min);
			state[0][n] = min;
			state[1][n] = nearest_place;
			n++;
			staff_position[0] = shop_address[0][nearest_place];
			staff_position[1] = shop_address[1][nearest_place];
			report_list[nearest_place] = 0;
		}
	}   

	//go to client home
	min = abs(staff_position[0] - client_address[0]) + abs(staff_position[1] - client_address[1]);
	insert_destination(delivery_man, client_address[0], client_address[1],  min);
	state[0][n] = min;
	state[1][n] = -1;
}

int calculate_cost(struct delivery_man_info delivery_man, int report_list[12], int client_address[2]){
	int time = 0;
	int i = 0;
	int staff_position[2] = {delivery_man.current_position[0], delivery_man.current_position[1]};

	int nearest_place = 0;
	int tmp_cost = 0;
	// int min_cost_tmp = 100;

	while(i<20){
		if(delivery_man.cost_time[i]){
			time += delivery_man.cost_time[i];
			staff_position[0] = delivery_man.destination[0][i];
			staff_position[1] = delivery_man.destination[1][i];
			i++;
		}
		else if(delivery_man.cost_time[i] == 0) break;
	}
	for(int j=0; j<12; j++)
	{
		int min = INT_MAX;
		bool check = false;
		for(int k=0; k<12; k++)
		{
			if(report_list[k])
			{
				check = true;
				int tmp =  abs(shop_address[0][k] - staff_position[0]) + abs(shop_address[1][k] - staff_position[1]);
				if(tmp < min)
				{
					min = tmp;
					nearest_place = k;
				}
			}
		}
		if(!check)
			break;
		else{
			time += min;
			staff_position[0] = shop_address[0][nearest_place];
			staff_position[1] = shop_address[1][nearest_place];
			report_list[nearest_place] = 0;
		}
	}

	time += abs(staff_position[0] - client_address[0]) + abs(staff_position[1] - client_address[1]);

	return time;
}

int choose_delivery_staff(int report_list[12], int client_address[2], int (*state)[50], int *sending_staff){
	int cost_time = 500;
	int choosed_staff = 0;

	for(int i=0; i<5; i++){
		int tmp_list[12] = {0};
		for(int j=0; j<12; j++){
			tmp_list[j] = report_list[j];
		}
		int tmp_cost = calculate_cost(staff[i], tmp_list, client_address);
		if(tmp_cost < cost_time){
			cost_time = tmp_cost;
			choosed_staff = i;
		}
	} 

	*sending_staff = choosed_staff;

	insert_into_staff_list(&staff[choosed_staff], report_list, client_address, state);

	return cost_time;
}
int recal_wait (int client_x, int client_y, int (*state)[50], struct delivery_man_info delivery_now, int cur_place, int sending_staff)
{
	int time = 0;
	state[0][cur_place] = 0;
	staff[sending_staff].cost_time[0] = 0;
	int add = 0;
	int x = delivery_now.current_position[0], y = delivery_now.current_position[1];

	int j = 1, i;
	for(i = cur_place+1; i < 50; i++)
	{
		if (state[1][i] == -1)
			break;
		
		add = abs(x - shop_address[0][state[1][i]]) + abs(y - shop_address[1][state[1][i]]);
		state[0][i] = add;
		staff[sending_staff].cost_time[j] = add;
		time += add;
		j++;
		x = shop_address[0][state[1][i]];
		y = shop_address[1][state[1][i]];	
	}

	add = abs(x-client_x) + abs(y-client_y);
	staff[sending_staff].cost_time[j] = add;
	state[0][i] = add;
	time += add;
	return time;


}
void report(char *op, int connfd){
	char feedback[1000] = "";
	char tmpp[256];
	strcpy(tmpp, op);
	int count = 0;
	char *op_sep[30];
	char *token = strtok(op, " ");
	while (token != NULL) {
		op_sep[count++] = token;
		token = strtok(NULL, " ");
	}

	int report_list[12] = {0};
	int client_address[2] = {0};
	int switch_flag = 0;
	for(int i=0; i<count; i++){
		if(strstr(op_sep[i], "(")) switch_flag = i;
		if(!switch_flag){
			int shop_cur = atoi(op_sep[i]);
			report_list[shop_cur-1] = 1;
		}
		else{
			client_address[0] = atoi(op_sep[i+1]);
			client_address[1] = atoi(op_sep[i+2]);
			break;
		}
	}
	if(strstr(op_sep[count-1] , ")") != NULL){
		count -= 4;
	}
	else{
		count -= 3;
	}
	
	if (client_address[0] <= 1 && client_address[1] >= 1)
	{
		printf("Received from %d client: at area A %s\n", connfd, tmpp);
	}
	else if ((client_address[0] <= 0 && client_address[1] < 1) || (client_address[0] <= 3 && client_address[1] <= -2))
	{
		printf("Received from %d client: at area B %s\n", connfd, tmpp);
	}
	else
	{
		printf("Received from %d client: at area C %s \n", connfd, tmpp);
	}
	int state[2][50] = {{0}};
	int sending_staff;
	int wait_time = choose_delivery_staff(report_list, client_address, state, &sending_staff);
	int cur_place = 0;
	char tmp[200];
	for (int j = 0; j < 50; j++) 
	{
		if (state[1][j] == -1)
		{
			break;
		}
		if(state[0][j]) 
		{
			led_state[state[1][j]][2] = 1;
		}
	}
	int count_tmp = 0 ;
	bool count_brk = false;
	while(wait_time)
	{

		sprintf(feedback, "====================\n");
		if(cur_place != 0){
			if(led_state[state[1][cur_place]][0] && state[1][cur_place] != -1)
			{
				led_state[state[1][cur_place]][1] = 0;
				led_state[state[1][cur_place]][2] = 0;
				count_tmp ++;
				wait_time = recal_wait(client_address[0], client_address[1], state, staff[sending_staff], cur_place, sending_staff);
				memset(feedback, 0, sizeof(feedback));
				sprintf(feedback, "************\nShop %s is out off electricity\n************\n", shop_name[state[1][cur_place]]);
				cur_place ++;
				if (send(connfd, feedback, 256, 0) < 0) 
				{
					perror("Failed to send response to client");
				} 
			}
			else
			{
				if(!state[0][cur_place])
					cur_place++;
				if(state[0][0])
					strcat(feedback, "Waiting for delivery man\n");
				else if(state[1][cur_place] == -1)
				{
					if(count_tmp == count){
						staff[sending_staff].cost_time[0] = 0;

						sprintf(tmp, "%s : YOU are not receiving food!!\n", delivery_staff[sending_staff]);
						strcat(feedback, tmp);
						wait_time = 0;
						count_brk = true;
					}
					else{				
						sprintf(tmp, "%s is heading to YOU\n", delivery_staff[sending_staff]);
						strcat(feedback, tmp);
					}

				}
				else
				{
					sprintf(tmp, "%s is heading to %s\n", delivery_staff[sending_staff], shop_name[state[1][cur_place]]);
					led_state[state[1][cur_place]][1] = 1;
					strcat(feedback, tmp);
				}
				sprintf(tmp, "Remain time : %d\n", wait_time);
				strcat(feedback, tmp);
				strcat(feedback, "====================\n");

				state[0][cur_place]--;
				if (state[0][cur_place] == 0 && state[1][cur_place] != -1)
				{
					led_state[state[1][cur_place]][1] = 0;
					led_state[state[1][cur_place]][2] = 0;
				}
				if (send(connfd, feedback, 256, 0) < 0) {
					perror("Failed to send response to client");
				} 
				if(!count_brk){
					wait_time--;
				}
			}
			
			
		}
		else
		{
			if(!state[0][cur_place])
				cur_place++;
			if(state[0][0])
				strcat(feedback, "Waiting for delivery man\n");
			else if(state[1][cur_place] == -1)
			{
				if(count_tmp == count){
					staff[sending_staff].cost_time[0] = 0;

					sprintf(tmp, "YOU are not receiving food!!\n");
					strcat(feedback, tmp);
					wait_time = 0;
					count_brk = true;
				}
				else{				
					sprintf(tmp, "%s is heading to YOU\n", delivery_staff[sending_staff]);
					strcat(feedback, tmp);
				}
			}
			else
			{
				sprintf(tmp, "%s is heading to %s\n", delivery_staff[sending_staff], shop_name[state[1][cur_place]]);
				led_state[state[1][cur_place]][1] = 1;
				strcat(feedback, tmp);
			}
			sprintf(tmp, "Remain time : %d\n", wait_time);
			strcat(feedback, tmp);
			strcat(feedback, "====================\n");

			state[0][cur_place]--;
			if (state[0][cur_place] == 0 && state[1][cur_place] != -1)
			{
				led_state[state[1][cur_place]][1] = 0;
				led_state[state[1][cur_place]][2] = 0;
			}
			if (send(connfd, feedback, 256, 0) < 0) {
				perror("Failed to send response to client");
			} 
			if(!count_brk){
				wait_time--;
			}
		}
		
		sleep(1);
	}

	if(!count_brk){
		memset(feedback, 0, 256);
		sprintf(feedback, "====================\n");
		strcat(feedback, "Your order has arrived!!!\n");
		strcat(feedback, "====================\n");
		if (send(connfd, feedback, 256, 0) < 0) {
			perror("Failed to send response to client");
		} 
	}
}

void move_position(int *cur_position_x, int *cur_position_y, int target_x, int target_y){
	if(target_x || target_y){
		int vector_x = target_x - *cur_position_x;
		int vector_y = target_y - *cur_position_y;
		if(vector_x > 0){
			(*cur_position_x) ++;
		}
		else if(vector_x < 0){
			(*cur_position_x) --;
		}
		else if(vector_y > 0){
			(*cur_position_y) ++;
		}
		else {
			(*cur_position_y) --;
		}
	}
}

void *time_controler(void* arg){

	while(1){
		for(int i=0; i<5; i++){
			if(staff[i].cost_time[0] > 0){
				move_position(&staff[i].current_position[0], &staff[i].current_position[1], staff[i].destination[0][0],  staff[i].destination[1][0]);
				staff[i].cost_time[0]--;

			}
			if(staff[i].cost_time[0] == 0){

				for(int j=0; j<19; j++){
					staff[i].cost_time[j] = staff[i].cost_time[j+1];
					staff[i].destination[0][j] = staff[i].destination[0][j+1];
					staff[i].destination[1][j] = staff[i].destination[1][j+1];
				}
				
				staff[i].cost_time[19] = 0;
				staff[i].destination[0][19] = 0;
				staff[i].destination[1][19] = 0;
				// pthread_mutex_unlock(&lock);
			}
		}
		sleep(1);
	}
}

void *connection_handler(void *socket_desc) {
	int sock = *(int *)socket_desc;

	// send welcom mwssage
	char welcom_msg[1024] = {0};
	sprintf(welcom_msg,"%s","Welcome to Foober!!! \nPlease select the restaurant you want to order\n");
	for(int i=0; i<12; i++){
		char tmp[10] = {0};
		sprintf(tmp, "%d", i+1);
		strcat(welcom_msg, tmp);
		strcat(welcom_msg, " ");
		strcat(welcom_msg, shop_name[i]);
		strcat(welcom_msg, "\n");
	}
	strcat(welcom_msg, "====================\n");
	strcat(welcom_msg, "Order food according to the flowing format : Shop list ( Adderss )\n");
	//printf("%s\n", welcom_msg);
	if (send(sock, welcom_msg, 256, 0) < 0) {
		perror("Failed to send response to client");
	} 

	while(1){
		char buffer[1024] = {0};
		char feedback[1024] = {0};
		int bytes_read = read(sock, buffer, 1024);
		if (bytes_read < 0) {
			perror("Failed to read message from client");
		} else {
			if(strstr(buffer, "search") != NULL)
			{
				printf("============================\n");
				for(int i=0; i < 5; i++)
				{
					printf("%s is at %d, %d\n", delivery_staff[i], staff[i].current_position[0], staff[i].current_position[1]);
				}
				printf("============================\n");
			}
			else
				report(buffer, sock);
		}
	}

	close(sock);
	signal(SIGCHLD, handler);

	return 0;
}

void button_handler()
{
	unsigned char a = 1;
	if(btn_state[0] == a)
	{
		printf("Area 1 power cut\n");
		for(int i = 0; i < 4; i++)
		{
			led_state[i][0] = 1;
		}
	}
	if(btn_state[1] == a)
	{
		printf("Area 2 power cut\n");
		for(int i = 4; i < 8; i++)
		{
			led_state[i][0] = 1;
		}
	}
	if(btn_state[2] == a)
	{
		printf("Area 3 power cut\n");
		for(int i = 8; i < 12; i++)
		{
			led_state[i][0] = 1;
		}
	}
	usleep(400000);
}

void *button_checker(void* arg)
{
	bool trig = false;

	while(1)
	{
		for(int i=0; i<3; i++)
		{
			if (bt[i] < 0) 
			{
				perror("Failed to open the device file");
				break;
			}
			if(read(bt[i], &btn_state[i], sizeof(btn_state[i])) < 0)
				break;
			if(btn_state[i])
			{
				trig = true;
			}
		}

		if(trig)
		{
			button_handler();
			trig = false;
		}
	}  
	usleep(50000);
}

void *seg_handler(void* arg)
{
	char buffer[256];
	while(1)
	{
		int area_a = 0; //北區 area1
		int area_b = 0; //東區 area2
		int area_c = 0; //香山區 area3
		for(int i = 0; i < 5; i++)
		{
			if (staff[i].current_position[0] <= 1 && staff[i].current_position[1] >= 1)
			{
				area_a++;
			}
			else if ((staff[i].current_position[0] <= 0 && staff[i].current_position[1] < 1) || (staff[i].current_position[0] <= 3 && staff[i].current_position[1] <= -2))
			{
				area_b++;
			}
			else
			{
				area_c++;
			}
		}
		
		system("echo A > /dev/seg");
		usleep(500000);
		sprintf(buffer, "echo %d > /dev/seg", area_a);
		system(buffer);
		usleep(500000);
		
		system("echo B > /dev/seg");
		usleep(500000);
		sprintf(buffer, "echo %d > /dev/seg", area_b);
		system(buffer);
		usleep(500000);
		
		system("echo C > /dev/seg");
		usleep(500000);
		sprintf(buffer, "echo %d > /dev/seg", area_c);
		system(buffer);
		usleep(500000);
		
		system("echo x > /dev/seg");
		sleep(1);
	}
}

void *led_checker(void* arg)
{
	ThreadArgs* args = (ThreadArgs*)arg;
	int led = args->value;
	bool shine = false;
	char buf[1024];
	int add;
	while(1)
	{
		if(led_state[led][0])
		{//emergency high frequency
			if(shine)
			{
				add = 109;
				if(led == 11)
				{
					add = 110;
				}
				sprintf(buf, "echo %c > /dev/led", led+add);
				system(buf);
				shine = false;
			}
			else
			{
				add = 97;
				sprintf(buf, "echo %c > /dev/led", led+add);
				system(buf);	
				shine = true;	
			}
			
			usleep(100000);
		}
		else if(led_state[led][1])
		{//heading low frequency
			if(shine)
			{
				add = 109;
				if(led == 11)
				{
					add = 110;
				}
				sprintf(buf, "echo %c > /dev/led", led+add);
				system(buf);
				shine = false;
			}
			else
			{
				add = 97;
				sprintf(buf, "echo %c > /dev/led", led+add);
				system(buf);	
				shine = true;	
			}
			
			usleep(500000);
		}
		else if(led_state[led][2])
		{// preparing
			add = 97;
			sprintf(buf, "echo %c > /dev/led", led+add);
			system(buf);
			usleep(500000);
		}
		else 
		{// unused
			add = 109;
			if(led == 11)
			{
				add = 110;
			}
			sprintf(buf, "echo %c > /dev/led", led+add);
			system(buf);
			usleep(500000);
		}
	}
}

void cleanup()
{
	for(int i=0; i<3; i++)
	{
		close(bt[i]);
	}
	exit(-1);
}

int main(int argc, char *argv[]) {

	pid = getpid();

	int server_socket, client_socket, c, *new_sock;
	struct sockaddr_in server, client;
	// initialize global variable
	// srand(time(NULL));
	for (int i=0; i<5; i++) {
		staff[i].current_position[0] = rand() % 25 - 12;
		staff[i].current_position[1] = rand() % 25 - 12;
		while(!staff[i].current_position[0] && !staff[i].current_position[1]){
			staff[i].current_position[0] = rand() % 25 - 12;
			staff[i].current_position[1] = rand() % 25 - 12;
		}
		for(int j=0; j<20; j++){
			staff[i].destination[0][j] = 0;
			staff[i].destination[1][j] = 0;
			staff[i].cost_time[j] = 0;
		}
	}

	// time control
	pthread_t sniffer_thread;
	if (pthread_create(&sniffer_thread, NULL, time_controler, NULL) < 0) {
		perror("could not create thread");
		return 1;
	}

	// interrupt initialize
	for(int i=0; i < 12; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			led_state[i][j] = 0;
		}
	}
	// button
	for(int i=0; i<3; i++)
	{
		bt[i] = open(BTN_PATH[i], O_RDONLY);
	}
	pthread_t button_thread;
	if (pthread_create(&button_thread, NULL, button_checker, NULL) < 0) {
		perror("could not create thread");
		return 1;
	}
	
	//Seg
	pthread_t seg_thread;
	if (pthread_create(&seg_thread, NULL, seg_handler, NULL) < 0) {
		perror("could not create thread");
		return 1;
	}
	
	//LED
	pthread_t led_thread[12];
	ThreadArgs* parameter = (ThreadArgs*)malloc(sizeof(ThreadArgs));
	for(int i = 0; i < 12; i++)
	{
		parameter->value = i;
		if (pthread_create(&led_thread[i], NULL, led_checker, parameter) < 0) 
		{
			perror("could not create thread");
			return 1;
		}
		parameter++;
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
		//printf("Connection accepted\n");

		pthread_t sniffe_thread;
		new_sock = (int*)malloc(sizeof(int));
		*new_sock = client_socket;

		if (pthread_create(&sniffe_thread, NULL, connection_handler, (void *)new_sock) < 0) {
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
