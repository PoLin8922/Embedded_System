#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

struct place_info{
	int child, adult, elder;
};

void print_info()
{
	//system("clear");
	printf("1. Search\n");
	printf("2. Report\n");
	printf("3. Exit\n");
}

int main()
{
	int op1;
	char op2;
	int area, num, total, div = 1;
	char a_r = 'c', age;
	struct place_info stadium = {.child = 0, .adult = 0, .elder = 0};
	struct place_info city = {.child = 0, .adult = 0, .elder = 0};
	struct place_info zoo = {.child = 0, .adult = 0, .elder = 0};
	system("echo z > /dev/etx_device");

	while(1)
	{
		print_info();
		scanf("%d", &op1);
		getchar();
		switch(op1)
		{
			case 1:
				while(1)
				{
					div = 1;
					if(stadium.child || stadium.adult || stadium.elder)
						system("echo a > /dev/etx_device");
					else
						system("echo d > /dev/etx_device");
					if(city.child || city.adult || city.elder)
						system("echo b > /dev/etx_device");
					else
						system("echo e > /dev/etx_device");
					if(zoo.child || zoo.adult || zoo.elder)
						system("echo c > /dev/etx_device");
					else
						system("echo f > /dev/etx_device");

					total = stadium.child + stadium.adult + stadium.elder + city.child + city.adult + city.elder + zoo.child + zoo.adult + zoo.elder;
					while(total/div >= 10)
					{
						div*=10;
					}
					while(div > 0)
					{
						int temp = total/div;
						char start[25]= "echo   > /dev/etx_device";
						char input = temp + '0';
						start[5] = input;
						system(start);
						total %= div;
						div /= 10;
						sleep(1);
						system("echo z > /dev/etx_device");
						usleep(500000);	
					}
					printf("1. Baseball Stadium : %d\n", stadium.child + stadium.adult + stadium.elder);
					printf("2. Big City : %d\n", city.child + city.adult + city.elder);
					printf("3. Zoo : %d\n", zoo.child + zoo.adult + zoo.elder);
					scanf("%c", &op2);
					getchar();
					//system("clear");
					if(op2 == 'q')
						break;
					switch(op2)
					{	    	    	
						case '1':	
							div = 1;
							for(int i=0; i<3; i++)
							{
								system("echo a > /dev/etx_device");
								usleep(500000);
								system("echo d > /dev/etx_device");
								usleep(500000);
							}
							system("echo a > /dev/etx_device");
							system("echo e > /dev/etx_device");
							system("echo f > /dev/etx_device");
							total = stadium.child + stadium.adult + stadium.elder;
							while(total/div >= 10)
							{
								div*=10;
							}
							while(div > 0)
							{
								int temp = total/div;
								char start[25]= "echo   > /dev/etx_device";
								char input = temp + '0';
								start[5] = input;
								system(start);
								total %= div;
								div /= 10;
								sleep(1);
								system("echo z > /dev/etx_device");
								usleep(500000);	
							}
							printf("Child : %d\n", stadium.child);
							printf("Adult : %d\n", stadium.adult);
							printf("Elder : %d\n", stadium.elder);
							getchar();
							break;
						case '2':
							div = 1;
							for(int i=0; i<3; i++)
							{
								system("echo b > /dev/etx_device");
								usleep(500000);
								system("echo e > /dev/etx_device");
								usleep(500000);
							}
							system("echo b > /dev/etx_device");
							system("echo d > /dev/etx_device");
							system("echo f > /dev/etx_device");
							total = city.child + city.adult + city.elder;
							while(total/div >= 10)
							{
								div*=10;
							}
							while(div > 0)
							{
								int temp = total/div;
								char start[25]= "echo   > /dev/etx_device";
								char input = temp + '0';
								start[5] = input;
								system(start);
								total %= div;
								div /= 10;
								sleep(1);
								system("echo z > /dev/etx_device");
								usleep(500000);	
							}
							printf("Child : %d\n", city.child);
							printf("Adult : %d\n", city.adult);
							printf("Elder : %d\n", city.elder);
							getchar();
							break;
						case '3':
							div = 1;
							for(int i=0; i<3; i++)
							{
								system("echo c > /dev/etx_device");
								usleep(500000);
								system("echo f > /dev/etx_device");
								usleep(500000);
							}
							system("echo c > /dev/etx_device");
							system("echo d > /dev/etx_device");
							system("echo e > /dev/etx_device");
							total = zoo.child + zoo.adult + zoo.elder;
							while(total/div >= 10)
							{
								div*=10;
							}
							while(div > 0)
							{
								int temp = total/div;
								char start[25]= "echo   > /dev/etx_device";
								char input = temp + '0';
								start[5] = input;
								system(start);
								total %= div;
								div /= 10;
								sleep(1);
								system("echo z > /dev/etx_device");
								usleep(500000);	
							}
							printf("Child : %d\n", zoo.child);
							printf("Adult : %d\n", zoo.adult);
							printf("Elder : %d\n", zoo.elder);
							getchar();
							break;
					}
				}
				break;
			case 2:
				a_r = 'c';
				while(1)
				{
					//system("clear");
					if(a_r == 'c')
					{
						printf("Area (1~3) : ");
						scanf("%d", &area);
						getchar();
						printf("Add or Reduce ('a' or 'r') : ");
						scanf("%c", &a_r);
						getchar();
						printf("Age group ('c', 'a', 'e') : ");
						scanf("%c", &age);
						getchar();
						printf("The number of passenger : ");
						scanf("%d", &num);
						getchar();
						div = 1;
						if(area == 1)
						{
							if(age == 'c')
							{
								a_r == 'a' ? (stadium.child += num) : (stadium.child -= num);
							}
							else if(age == 'a')
							{
								a_r == 'a' ? (stadium.adult += num) : (stadium.adult -= num);
							}
							else
							{
								a_r == 'a' ? (stadium.elder += num) : (stadium.elder -= num);
							}
							for(int i=0; i<3; i++)
							{
								system("echo a > /dev/etx_device");
								usleep(500000);
								system("echo d > /dev/etx_device");
								usleep(500000);
							}
							if(stadium.child || stadium.adult || stadium.elder)
								system("echo a > /dev/etx_device");
							total = stadium.child + stadium.adult + stadium.elder;

							while(total/div >= 10)
							{
								div*=10;
							}
							while(div > 0)
							{
								int temp = total/div;
								char start[25]= "echo   > /dev/etx_device";
								char input = temp + '0';
								start[5] = input;
								system(start);
								total %= div;
								div /= 10;
								sleep(1);
								system("echo z > /dev/etx_device");
								usleep(500000);	
							}
						}
						else if(area == 2)
						{
							if(age == 'c')
							{
								a_r == 'a' ? (city.child += num) : (city.child -= num);
							}
							else if(age == 'a')
							{
								a_r == 'a' ? (city.adult += num) : (city.adult -= num);
							}
							else
							{
								a_r == 'a' ? (city.elder += num) : (city.elder -= num);
							}
							for(int i=0; i<3; i++)
							{
								system("echo b > /dev/etx_device");
								usleep(500000);
								system("echo e > /dev/etx_device");
								usleep(500000);
							}
							if(city.child || city.adult || city.elder)
								system("echo b > /dev/etx_device");
							total = city.child + city.adult + city.elder;
							while(total/div >= 10)
							{
								div*=10;
							}
							while(div > 0)
							{
								int temp = total/div;
								char start[25]= "echo   > /dev/etx_device";
								char input = temp + '0';
								start[5] = input;
								system(start);
								total %= div;
								div /= 10;
								sleep(1);
								system("echo z > /dev/etx_device");
								usleep(500000);	
							}
						}
						else if(area == 3)
						{
							if(age == 'c')
							{
								a_r == 'a' ? (zoo.child += num) : (zoo.child -= num);
							}
							else if(age == 'a')
							{
								a_r == 'a' ? (zoo.adult += num) : (zoo.adult -= num);
							}
							else
							{
								a_r == 'a' ? (zoo.elder += num) : (zoo.elder -= num);
							}
							for(int i=0; i<3; i++)
							{
								system("echo c > /dev/etx_device");
								usleep(500000);
								system("echo f > /dev/etx_device");
								usleep(500000);
							}
							if(zoo.child || zoo.adult || zoo.elder)
								system("echo c > /dev/etx_device");
							total = zoo.child + zoo.adult + zoo.elder;
							while(total/div >= 10)
							{
								div*=10;
							}
							while(div > 0)
							{
								int temp = total/div;
								char start[25]= "echo   > /dev/etx_device";
								char input = temp + '0';
								start[5] = input;
								system(start);
								total %= div;
								div /= 10;
								sleep(1);
								system("echo z > /dev/etx_device");
								usleep(500000);	
							}
						}
						printf("\n(Exit or Continue)\n");
						scanf("%c", &a_r);
						getchar();
					}
					else
						break;
				}
				break;
			case 3:
				system("echo z > /dev/etx_device");
				system("echo d > /dev/etx_device");
				system("echo e > /dev/etx_device");
				system("echo f > /dev/etx_device");
				exit(-1);
		}
	}

	return 0;
}
