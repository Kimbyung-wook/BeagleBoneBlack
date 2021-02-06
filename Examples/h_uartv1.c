/*
 * Author : David Kim
 *
 * data : 15.11.03
 *
 * This code can use gpio, pwm
 * use Custom Header
 */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "CustomHeaderv1.h"

int main()
{
	int i;
	int cnt;
	int UART1_News=1;
	int UART2_News=1;
	int readfromUART1 = 0;
	int readfromUART2 = 0;
	int UART1T=0;
	int UART1R=0;
	int UART2T=0;
	int UART2R=0;
	char buf[100] = {'\0',};
	char buf1[100] = {'\0',};
	char UART1_data[300]; //AHRS1
	char UART2_data[300]; //AHRS2

	LEDOn(23);
	LEDOn(47);
	InitPin(27);
	SetPinDirt(27,IN);

	printf("Start\n");

	UART1T=UART_init(1,'T');
	printf("UART1T : %d\n",UART1T);
	UART1R=UART_init(1,'R');
	printf("UART1R : %d\n",UART1R);
	UART_Baudrate_NonC(UART1T,9600,0,1);
/*
	UART1T=UART_init(2,'T');
	printf("UART1T : %d\n",UART1T);
	UART1R=UART_init(2,'R');
	printf("UART1R : %d\n",UART1R);
	UART_Baudrate_NonC(UART2T,9600,0,1);
*/
	printf("START!\n");
	write(UART1T,"START!\n",8);
	while(!GetPinValue(27))
	{
		readfromUART1 = read(UART1R,buf,100);
		//readfromUART1 = read(UART1R,buf,100);
		if(readfromUART1 != -1)	//문자를 받아왔을 경우
		{
			//UART1_data
			printf("|%s|%i|\n",buf,readfromUART1);
			Nullize(buf,readfromUART1);
		}
		else
		{
			usleep(200000);
			write(UART1T,"z",2);
		}

		/*if(UART1_News&&)		//정보 수신중에는 다른짓 못하게 해라
		{

		}*/
	}
	//UART_Close(1,'T');
	//UART_Close(1,'R');
	write(UART1T,"End\n",5);
	printf("End\n");
	CloseGPin(27);
	LEDOff(23);
	LEDOff(47);
	return 0;
}
