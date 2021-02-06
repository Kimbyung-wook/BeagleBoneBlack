/*
 * This is header file for beaglebone black as using cape-universalk for device tree overlay
 *
 * Author : David Kim
 *
 * data : 15.11.11
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>		//Hard link and Symbolic link
#include <termios.h>	//UART Setting
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <stdio_ext.h>

#define OUT "out"
#define IN "in"
#define ON 1
#define OFF 0
#define TRUE 1
#define FALSE 0

//**********************************************************
//*                      Cape Function                     *
//**********************************************************
int SettingCape(void)
{
	FILE * io_fp;
	io_fp = fopen("/sys/devices/bone_capemgr.9/slots","w");
	if(io_fp == NULL) printf("This cape cant enter");
	fseek(io_fp,0,SEEK_SET);
	fprintf(io_fp,"%s","cape-universalk");
	fflush(io_fp);
	fclose(io_fp);
	return 0;
}
//**********************************************************
//*                      GPIO Function                     *
//**********************************************************
int InitPin(int GPIO_num)
{
	FILE *GPIO_fp_addr;

	GPIO_fp_addr = fopen("/sys/class/gpio/export","w");						//file open - gpio-export
	if(GPIO_fp_addr == NULL) printf("Pin Failed to initialize_InitPin\n");	//if fail to open, print
	fseek(GPIO_fp_addr,0,SEEK_SET);											//go FilePointer to first
	fprintf(GPIO_fp_addr,"%d",GPIO_num);										//Open GPIO_num
	fflush(GPIO_fp_addr);													//FilePointer clear
	fclose(GPIO_fp_addr);													//File close

	return 0;
}
int SetPinDirt(int GPIO_num, char* GPIO_InOut)						//'GPIO_InOut' is direction (in or out)
{
	FILE *GPIO_fp_addr;
	char GPIO_way[15];												//input "direction"
	char GPIO_addr[50] = "/sys/class/gpio/gpio";

	sprintf(GPIO_way,"%i",GPIO_num);								//duplicate GPIO_num to GPIO_way(memory)
	strcat(GPIO_addr,strcat(GPIO_way,"/direction"));				//maGPIO: GPIO_addr + GPIO_num + "/direction"

	GPIO_fp_addr = fopen(GPIO_addr,"w");									//open adress 'GPIO_addr' on writing condition
	if(GPIO_fp_addr == NULL) printf("Direction failed to open\n");		//if fail to open, print
	fseek(GPIO_fp_addr,0,SEEK_SET);										//go FilePointer to first
	fprintf(GPIO_fp_addr,"%s",GPIO_InOut);								//printf 'GPIO_InOut' to filepointer 'GPIO_fp_addr'
	fflush(GPIO_fp_addr);
	fclose(GPIO_fp_addr);

	return 0;
}
int SetPinValue(int GPIO_num, int GPIO_value)
{
	FILE *GPIO_fp_addr;
	char GPIO_val[10];											//input "GPIO_value"
	char GPIO_addr[50] = "/sys/class/gpio/gpio";

	sprintf(GPIO_val,"%i",GPIO_num);								//duplicate GPIO_num to buf(memory)
	strcat(GPIO_addr,strcat(GPIO_val,"/value"));						//make : buf2 + GPIO_num + "/value"

	GPIO_fp_addr = fopen(GPIO_addr,"w");
	if(GPIO_fp_addr == NULL) printf("Value failed to open\n");
	fseek(GPIO_fp_addr,0,SEEK_SET);
	fprintf(GPIO_fp_addr,"%d",GPIO_value);
	fflush(GPIO_fp_addr);
	fclose(GPIO_fp_addr);

	return 0;
}
int GetPinValue(int GPIO_num)
{
	FILE *GPIO_fp_addr;								//file pointer address
	int GPIO_value;									//GPIO pin value
	char GPIO_val[5];								//GPIO pin char array
	char GPIO_addr[50] = "/sys/class/gpio/gpio";	//GPIO address

	//build file path to value file
	sprintf(GPIO_val,"%i",GPIO_num);				//GPIO pin number to GPIO pin char array
	strcat(GPIO_addr,strcat(GPIO_val,"/value"));	//GPIO address

	GPIO_fp_addr = fopen(GPIO_addr, "r");
	if(GPIO_fp_addr == NULL) printf("Input value failed to open\n");
	fseek(GPIO_fp_addr,0,SEEK_SET);
	fscanf(GPIO_fp_addr,"%d",&GPIO_value);
	fclose(GPIO_fp_addr);

	return GPIO_value;
}
int LEDOn(int GPIO_num)
{
	InitPin(GPIO_num);
	SetPinDirt(GPIO_num,OUT);
	SetPinValue(GPIO_num,ON);
	return 1;
}
int LEDOff(int GPIO_num)
{
	SetPinValue(GPIO_num,OFF);
	CloseGPin(GPIO_num);
	return 1;
}
int CloseGPin(int GPIO_num)
{
	FILE *GPIO_fp_addr;

	GPIO_fp_addr = fopen("/sys/class/gpio/unexport","w");
	if(GPIO_fp_addr == NULL) printf("Pin Failed to initialize_ClosePin\n");
	fseek(GPIO_fp_addr,0,SEEK_SET);
	fprintf(GPIO_fp_addr,"%d",GPIO_num);
	fflush(GPIO_fp_addr);
	fclose(GPIO_fp_addr);

	return 0;
}
//*************************************************
//*                PWM FUNCTIONS                  *
//*************************************************
int InitPWM(int PWM_num)
{
	//Function of this file is setting pwm port
	//if you want to use export number 4,
	//this function is entered to 4
	//1:echo pwm > /sys/devices/ocp.3/P9_14_pinmux.32/state
	//2:echo 3 > /sys/class/pwm/export
	//1:echo pwm > /sys/devices/ocp.3/P9_16_pinmux.34/state
	//2:echo 4 > /sys/class/pwm/export
	//3:echo 1000000000 > /sys/class/pwm/pwm3/period_ns
	//4:echo 500000000 > /sys/class/pwm/pwm3/duty_ns
	//5:echo 1 > /sys/class/pwm/pwm3/run
	FILE *PWM_set_addr;
	FILE *PWM_export;
	char PWM_pin_addr1[50] = "/sys/devices/ocp.3";
	char PWM_pin_addr2[25] = {'\0',};
	char PWM_pin_num[5]={'\0',};
	//char buf[15];// /P9_14_pinmux.32 선언
	// slot에 잘 연결 안되면 segmentation fault 뜸

	switch(PWM_num)
	{
	case 0 :
		strcpy(PWM_pin_addr2,"/P9_29_pinmux.44"); //for cape-universalk
		break;
	case 1 :
		strcpy(PWM_pin_addr2,"/P9_31_pinmux.46"); //for cape-universalk
		break;
	case 3 :
		strcpy(PWM_pin_addr2,"/P9_14_pinmux.33"); //for cape-universalk
		break;
	case 4 :
		strcpy(PWM_pin_addr2,"/P9_16_pinmux.35"); //for cape-universalk
		break;
	case 5 :
		strcpy(PWM_pin_addr2,"/P8_45_pinmux.28"); //for cape-universalk
		break;
	case 6 :
		strcpy(PWM_pin_addr2,"/P8_46_pinmux.29"); //for cape-universalk
		break;
	default :
		printf("Error!\n");
		break;
	}
	strcat(PWM_pin_addr1,strcat(PWM_pin_addr2,"/state"));
		// /sys/devices/ocp.3 /P9_14_pinmux.32 /state
	PWM_set_addr = fopen(PWM_pin_addr1,"w");
	if(PWM_set_addr == NULL) printf("Init_PWM failed to setting");
	fseek(PWM_set_addr,0,SEEK_SET);
	fprintf(PWM_set_addr,"pwm");
	printf("pwm > %s\n",PWM_pin_addr1);
		//1:echo pwm > /sys/devices/ocp.3/P9_14_pinmux.32/state
	fflush(PWM_set_addr);
	fclose(PWM_set_addr);

	PWM_export = fopen("/sys/class/pwm/export","w");
	if(PWM_export == NULL) printf("Init_PWM failed to export");
	sprintf(PWM_pin_num,"%i",PWM_num);
	fprintf(PWM_export,PWM_pin_num);
		//2:echo 3 > /sys/class/pwm/export
	fflush(PWM_export);
	fclose(PWM_export);

	return 1;
}
int SetPWMPeriod(int PWM_num,int period)
{
	// Motor period : 10ms (100Hz)
	// Servo period : 2.5 ~ 20ms (50 ~ 400Hz)
	FILE *PWM_period_addr;
	char PWM_pin_num[5];
	char PWM_addr[60] = "/sys/class/pwm/pwm";
	//3:echo 1000000000 > /sys/class/pwm/pwm3/period_ns

	sprintf(PWM_pin_num,"%i",PWM_num);
	strcat(strcat(PWM_addr,PWM_pin_num),"/period_ns");
	// /sys/class/pwm/pwm3/period_ns

	PWM_period_addr = fopen(PWM_addr, "w");
	if(PWM_period_addr == NULL) printf("PWM Period failed to open\n");
	fseek(PWM_period_addr,0,SEEK_SET);
	fprintf(PWM_period_addr,"%d",period);
	printf("%d > %s\n",period,PWM_addr);
	fflush(PWM_period_addr);
	fclose(PWM_period_addr);

	return 0;
}
int SetPWMDuty(int PWM_num,double duty)
{
	FILE *PWM_duty_addr;
	FILE *PWM_period_addr;
	char PWM_pin_num[5];
	char PWM_period[15] = {'\0',};
	char PWM_addr[60] = "/sys/class/pwm/pwm";
	//4:echo 500000000 > /sys/class/pwm/pwm3/duty_ns

	sprintf(PWM_pin_num,"%i",PWM_num);
	strcat(strcat(PWM_addr,PWM_pin_num),"/period_ns");
	PWM_period_addr = fopen(PWM_addr,"r");
	fread(PWM_period,1,15,PWM_period_addr);

	strcpy(PWM_addr,"/sys/class/pwm/pwm");
	sprintf(PWM_pin_num,"%i",PWM_num);
	strcat(strcat(PWM_addr,PWM_pin_num),"/duty_ns");

	if(duty > 100.0) duty = 100.0;
	if(duty < 0.0 ) duty = 0.0;
	// /sys/class/pwm/pwm3/duty_ns
	PWM_duty_addr = fopen(PWM_addr, "w");
	if(PWM_duty_addr == NULL) printf("PWM Duty failed to open\n");
	fseek(PWM_duty_addr,0,SEEK_SET);
	fprintf(PWM_duty_addr,"%i",(int)((duty/100)*((double)atoi(PWM_period))));
	fflush(PWM_duty_addr);
	fclose(PWM_duty_addr);

	return 0;
}
int SetPWMOnOff(int PWM_num, int run)
{
	FILE *PWM_onoff_addr;
	char PWM_pin_num[5];
	char PWM_addr[50] = "/sys/class/pwm/pwm";
	sprintf(PWM_pin_num,"%i",PWM_num);
	strcat(strcat(PWM_addr,PWM_pin_num),"/run");

	PWM_onoff_addr = fopen(PWM_addr, "w");
	if(PWM_onoff_addr == NULL) printf("PWM Run failed to open\n");
	fseek(PWM_onoff_addr,0,SEEK_SET);
	fprintf(PWM_onoff_addr,"%i",run);
	printf("%i > %s\n",run,PWM_addr);
	fflush(PWM_onoff_addr);
	fclose(PWM_onoff_addr);

	return 0;
}
int ClosePWMPin(int GPIO_num)
{
	FILE *PWM_unexport;

	PWM_unexport = fopen("/sys/class/pwm/unexport","w");
	if(PWM_unexport == NULL) printf("PWM Failed to initialize_ClosePWMPin\n");
	fseek(PWM_unexport,0,SEEK_SET);
	fprintf(PWM_unexport,"%d",GPIO_num);
	fflush(PWM_unexport);
	fclose(PWM_unexport);

	return 0;
}
//*************************************************
//*                UART Setting                   *
//*************************************************
int UART_init(int UART_num,char TRX)
{
	int tranceiver;			//File discripter
							//This number is returned for
	int UARTpin;
	char UARTSetAddr[50] = "/sys/devices/ocp.3";
	char tty[15] = {'\0',};

	//UART initial setting - pinmux


	switch(TRX)
	{
		case 'T' :
		{
			switch(UART_num)
			{
			case 0 :
				printf("Number 0 is not export number\n");
				break;
			case 1 :
				//echo uart > /sys/devices/ocp.3/P9_26_pinmux.42/state
				strcat(UARTSetAddr,"/P9_24_pinmux.41/state"); //for cape-universalk
				break;
			case 2 :
				strcat(UARTSetAddr,"/P9_21_pinmux.38/state"); //for cape-universalk
				break;
			case 3 :
				printf("Number T3 is not export number\n");
				break;
			case 4 :
				strcat(UARTSetAddr,"/P9_13_pinmux.32/state"); //for cape-universalk
				break;
			case 5 :
				strcat(UARTSetAddr,"/P8_37_pinmux.26/state"); //for cape-universalk
				break;
			default :
				printf("Out of export number\n");
				break;
			}
			break;
		}
		case 'R' :
		{
			switch(UART_num)
			{
			case 0 :
				printf("Number 0 is not export number\n");
				break;
			case 1 :
				//echo uart > /sys/devices/ocp.3/P9_26_pinmux.42/state
				strcat(UARTSetAddr,"/P9_26_pinmux.42/state"); //for cape-universalk
				break;
			case 2 :
				strcat(UARTSetAddr,"/P9_22_pinmux.39/state"); //for cape-universalk
				break;
			case 3 :
				printf("Number R3 is not export number\n");
				break;
			case 4 :
				strcat(UARTSetAddr,"/P9_11_pinmux.30/state"); //for cape-universalk
				break;
			case 5 :
				strcat(UARTSetAddr,"/P8_38_pinmux.27/state"); //for cape-universalk
				break;
			default :
				printf("Out of export number\n");
				break;
			}
			break;
		}
		default :
			printf("Error : Input data is invalid\n");
			break;
	}
	//echo uart > /sys/devices/ocp.3/P9_26_pinmux.42/state
	UARTpin = open(UARTSetAddr,O_WRONLY | S_IWRITE);

	printf("echo uart > %s\n",UARTSetAddr);

	if (UARTpin>0)	// set pinmux
	{
		write(UARTpin,"uart",4);
	}
	else
		printf("Failed to setting UART-pinmux!\n");
	// end initial setting
	printf("Setting : UART%i\n",UART_num);
	sprintf(tty,"/dev/ttyO%d",UART_num);
	//UART TRX
	tranceiver = open(tty,O_RDWR | O_NOCTTY | O_NONBLOCK);

	return tranceiver;
}
int UART_Baudrate_NonC(int tranceiver, int Baudrate,double C_VTIME,int C_VMIN)
{
	int Baudrate_d=0;
	struct termios newtio;

	switch(Baudrate)
	{
	case 150 : Baudrate_d = 5; break;
	case 200 : Baudrate_d = 6; break;
	case 300 : Baudrate_d = 7; break;
	case 600 : Baudrate_d = 8; break;
	case 1200 : Baudrate_d = 9; break;
	case 1800 : Baudrate_d = 10; break;
	case 2400 : Baudrate_d = 11; break;
	case 4800 :	Baudrate_d = 12; break;
	case 9600 :	Baudrate_d = 13; break;
	case 19200 : Baudrate_d = 14; break;
	case 38400 : Baudrate_d = 15; break;
	case 57600 : Baudrate_d = 4097; break;
	case 115200 : Baudrate_d = 4098; break;
	case 230400 : Baudrate_d = 4099; break;
	case 460800 : Baudrate_d = 4100; break;
	case 500000 :Baudrate_d = 4101; break;
	case 576000 : Baudrate_d = 4102; break;
	case 921600 : Baudrate_d = 4103; break;
	case 1000000 : Baudrate_d = 4104; break;
	case 1152000 : Baudrate_d = 4105; break;
	case 1500000 : Baudrate_d = 4106; break;
	case 2000000 : Baudrate_d = 4107; break;
	case 2500000 : Baudrate_d = 4108; break;
	case 3000000 : Baudrate_d = 4109; break;
	}

	memset( &newtio, 0, sizeof(newtio) );  // 시리얼 포트 통신 환경 설정
	newtio.c_cflag       = Baudrate_d | CS8 | CLOCAL | CREAD;
	newtio.c_iflag       = 0;              // NO_PARITY
	newtio.c_oflag       = 0;
	newtio.c_lflag       = 0;
	newtio.c_cc[VTIME]   = C_VTIME;
	newtio.c_cc[VMIN]    = C_VMIN;
	tcflush(tranceiver,TCIFLUSH);		//RXD1를
	tcsetattr(tranceiver, TCSANOW, &newtio); //즉시 적용한다.
	fcntl(tranceiver, F_SETFL, FNDELAY);
}
void UART_Close(int UART_num, char TRX)
{
	int UARTpin;
	char UARTSetAddr[50] = "/sys/devices/ocp.3/";
	char tty[15] = {'\0',};

	switch(TRX)
	{
		case 'T' :
		{
			switch(UART_num)
			{
			case 0 :
				printf("Number 0 is not export number\n");
				break;
			case 1 :
				//echo uart > /sys/devices/ocp.3/P9_26_pinmux.42/state
				strcat(UARTSetAddr,"/P9_24_pinmux.41/state"); //for cape-universalk
				break;
			case 2 :
				strcat(UARTSetAddr,"/P9_21_pinmux.38/state"); //for cape-universalk
				break;
			case 3 :
				printf("Number T3 is not export number\n");
				break;
			case 4 :
				strcat(UARTSetAddr,"/P9_13_pinmux.32/state"); //for cape-universalk
				break;
			case 5 :
				strcat(UARTSetAddr,"/P8_37_pinmux.26/state"); //for cape-universalk
				break;
			default :
				printf("Out of export number\n");
				break;
			}
			break;
		}
		case 'R' :
		{
			switch(UART_num)
			{
			case 0 :
				printf("Number 0 is not export number\n");
				break;
			case 1 :
				//echo uart > /sys/devices/ocp.3/P9_26_pinmux.42/state
				strcat(UARTSetAddr,"/P9_26_pinmux.42/state"); //for cape-universalk
				break;
			case 2 :
				strcat(UARTSetAddr,"/P9_22_pinmux.38/state"); //for cape-universalk
				break;
			case 3 :
				printf("Number R3 is not export number\n");
				break;
			case 4 :
				strcat(UARTSetAddr,"/P9_11_pinmux.30/state"); //for cape-universalk
				break;
			case 5 :
				strcat(UARTSetAddr,"/P8_38_pinmux.27/state"); //for cape-universalk
				break;
			default :
				printf("Out of export number\n");
				break;
			}
			break;
		}
		default :
			printf("Error : Input data is invalid\n");
			break;
	}
	//echo uart > /sys/devices/ocp.3/P9_26_pinmux.42/state
	UARTpin = open(UARTSetAddr,O_WRONLY|S_IWRITE);

	if (UARTpin>0)	// set pinmux
	{
		write(UARTpin,"default",7);
	}
	else
		printf("Failed to setting UART-pinmux!\n");
	// end initial setting
	printf("Setting default : UART1\n");
}
//*************************************************
//*               Custum Function                 *
//*************************************************
double DecaPower(int power)
{
	int i=0;
	double output = 1;
	for(i=1;i<power;i++)
	{
		output=output*10;
	}
	return output;
}
void Nullize(char *string,int size)
{
	int i=0;
	//printf("-%d-",size);
	for(i=0;i<size;i++)
	{
		string[i]='\0';
	}
}
