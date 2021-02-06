/*
 * myuart.c
 *
 *  Created on: 2015. 10. 5.
 *      Author: home
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio_ext.h>


int main(void)
{
	int fd1=open("/sys/devices/bone_capemgr.9/slots",O_WRONLY|S_IWRITE);
	int fd2 = 0;	//9_21 UART2 TX
	int fd3 = 0;	//9_22 UART2 RX
	int fd4 = 0;	//9_24 UART1 TX
	int fd5 = 0;	//9_26 UART1 RX
	int fd6 = 0;	//UART1 ttyO
	int fd7 = 0;	//UART2 ttyO
	int fd8 = 0;	//save data
	int readn = 0;	//size of data(byte)
	int MAX=10;
	char buf[MAX];
	char sen[MAX];
    int count=1;
    int i=0;


	if (fd1>0)		// equip cape mgr
	{
		write(fd1,"cape-universaln",16);
		printf("can use cape-universaln \n");
		fd2=open("/sys/devices/ocp.3/P9_21_pinmux.37/state",O_WRONLY|S_IWRITE);
		fd3=open("/sys/devices/ocp.3/P9_22_pinmux.38/state",O_WRONLY|S_IWRITE);
		fd4=open("/sys/devices/ocp.3/P9_24_pinmux.40/state",O_WRONLY|S_IWRITE);
		fd5=open("/sys/devices/ocp.3/P9_26_pinmux.41/state",O_WRONLY|S_IWRITE);
		printf("can use pinmux \n");
		if (fd2>0 && fd3>0 && fd4>0 && fd5>0)	// set pinmux
		{
			write(fd2,"uart",4);
			write(fd3,"uart",4);
			write(fd4,"uart",4);
			write(fd5,"uart",4);
		}
		else
			printf("error from uart-pinmux \n");
	}
	else
		 printf("error from fd1-cape \n");




	if (fd1>0 && fd2>0 && fd3>0 && fd4>0 && fd5>0)
	{
		fd6=open("/dev/ttyO1",O_RDWR|O_NOCTTY|O_NONBLOCK);
		fd7=open("/dev/ttyO2",O_RDWR|O_NOCTTY|O_NONBLOCK);
		fd8=open ("/home/debian/mycode/uartdata.txt" ,O_CREAT|O_RDWR|O_TRUNC, 0644);	// if needed, change O_TRUNC
		if (fd6>0 && fd7>0 && fd8>0)
		{

			while (i<1)
			{
				printf("\n write any sentence = \n");
				scanf("%s",&sen);
				printf ("input is : %s\n",sen);
				write(fd6,sen,sizeof(sen));
					printf("\n -----------------------------------(1) = \n");
				readn = read(fd7, buf, MAX);
					printf("\n -----------------------------------(2) = \n");
				printf ("--------------------\n\n");
					printf("\n -----------------------------------(3) = \n");
				printf ("Buffer Size : %d\n",sizeof(buf));
					printf("\n -----------------------------------(4) = \n");
				printf ("readn Size : %d\n",sizeof(readn));
					printf("\n -----------------------------------(5) = \n");
				printf ("Readn is ..... %d\n", readn);
					printf("\n -----------------------------------(6) = \n");
				printf ("sentence is    %s\n",buf);
					printf("\n -----------------------------------(7) = \n");
				//write(fd7,buf,readn);
				printf( "data received - %d %s\n", readn, buf);

			//	write(fd8,buf,readn);	//save our data
					printf("\n -----------------------------------(8) = \n");
				__fpurge(stdin);

				printf("do you want to continue? (n=0) \n");
				scanf("%d",&count);
				if (count==0)
				{
					i=i+1;
				}
				__fpurge(stdin);
			}
		}
		close(fd1);
		close(fd2);
		close(fd3);
		close(fd4);
		close(fd5);
		close(fd6);
		close(fd7);
		close(fd8);
		printf("finish our program \n");
	}
	else
		printf("error from fd5 6 7 8 -pwm \n");

	return 0;
}
