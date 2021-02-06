/*
 * This is header file for beaglebone black as using cape-universalk for device tree overlay
 *
 * Author : David Kim
 *
 * data : 15.11.11
 */


#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

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

int SetPWMDuty(int PWM_num,int duty)
{
	FILE *PWM_duty_addr;
	char PWM_pin_num[5];
	char PWM_addr[60] = "/sys/class/pwm/pwm";
	//4:echo 500000000 > /sys/class/pwm/pwm3/duty_ns

	sprintf(PWM_pin_num,"%i",PWM_num);
	strcat(strcat(PWM_addr,PWM_pin_num),"/duty_ns");

	// /sys/class/pwm/pwm3/duty_ns
	PWM_duty_addr = fopen(PWM_addr, "w");
	if(PWM_duty_addr == NULL) printf("PWM Duty failed to open\n");
	fseek(PWM_duty_addr,0,SEEK_SET);
	fprintf(PWM_duty_addr);
	printf("%d > %s\n",duty,PWM_addr);
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
	fprintf(PWM_onoff_addr,"%d",run);
	printf("%d > %s\n",run,PWM_addr);
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
int InitUART(int UART_num,int baudrate,char TRX)
{
	//예시는 아래와 같다.
	// fopen 종류와 open 종류중에서 open에 맞춘다.
	//Choose UART & Baudrate return file discripter and file pointer
	//echo uart > /sys/devices/ocp.3/P9_26_pinmux.42/state

	addr_RXD1 = fopen("/sys/devices/ocp.3/P9_26_pinmux.42/state","w");
	if(addr_RXD1 == NULL) printf("RXD1 Pin fail to set\n");
	fseek(addr_RXD1,0,SEEK_SET);
	fprintf(addr_RXD1,"%s","uart");
	fflush(addr_RXD1);
	fclose(addr_RXD1);
	printf("11st step\n");
	RXD1 = fopen("/dev/ttyO1","r");	//if reading, "r", but R/W, "r+"
	if(RXD1 == NULL) printf("RXD1 Port is not open!\n");
	fseek(RXD1,0,SEEK_SET);
	printf("12nd step\n");

	//여기서는 다른 헤더의 명령어 세트를 사용했다.
	RXD1i = open("/dev/ttyO1",O_RDWR | O_NOCTTY | O_NONBLOCK);
	memset( &newtio, 0, sizeof(newtio) );  // 시리얼 포트 통신 환경 설정
	newtio.c_cflag       = B115200 | CS8 | CLOCAL | CREAD;
	newtio.c_iflag       = 0;              // NO_PARITY
	newtio.c_oflag       = 0;
	newtio.c_lflag       = 0;
	newtio.c_cc[VTIME]   = 0;
	newtio.c_cc[VMIN]    = 1;
	tcflush(RXD1i,TCIFLUSH);		//RXD1를
	tcsetattr(RXD1i, TCSANOW, &newtio); //즉시 적용한다.
	fcntl(RXD1i, F_SETFL, FNDELAY);
	//******************************************************//

	int tranceive;			//File discripter
							//This number is returned for
	int UARTpin;
	char UARTSetAddr[50] = "/sys/devices/ocp.3/";
	char tty[10] = "/dev/ttyO";

	switch(TRX)
	{
	case 'T' :
	{
		switch(expnum)
		{
		case 0 :
			printf("Number 0 is not export number\n");
			break;
		case 1 :
			//echo uart > /sys/devices/ocp.3/P9_26_pinmux.42/state
			strcpy(UARTSetAddr,"/P9_24_pinmux.41/state"); //for cape-universalk
			break;
		case 2 :
			strcpy(UARTSetAddr,"/P9_21_pinmux.38/state"); //for cape-universalk
			break;
		case 3 :
			printf("Number 3 is not export number\n");
			break;
		case 4 :
			strcpy(UARTSetAddr,"/P9_11_pinmux.30/state"); //for cape-universalk
			break;
		case 5 :
			strcpy(UARTSetAddr,"/P8_37_pinmux.26/state"); //for cape-universalk
			break;
		default :
			printf("Out of export number\n");
			break;
		}
		break;
	}

	case 'R' :
	{
		switch(expnum)
		{
		case 0 :
			printf("Number 0 is not export number\n");
			break;
		case 1 :
			//echo uart > /sys/devices/ocp.3/P9_26_pinmux.42/state
			strcpy(UARTSetAddr,"/P9_26_pinmux.42/state"); //for cape-universalk
			break;
		case 2 :
			strcpy(UARTSetAddr,"/P9_22_pinmux.38/state"); //for cape-universalk
			break;
		case 3 :
			printf("Number 3 is not export number\n");
			break;
		case 4 :
			strcpy(UARTSetAddr,"/P9_11_pinmux.30/state"); //for cape-universalk
			break;
		case 5 :
			strcpy(UARTSetAddr,"/P8_37_pinmux.26/state"); //for cape-universalk
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
		write(fd2,"uart",4);
		write(fd3,"uart",4);
		write(fd4,"uart",4);
		write(fd5,"uart",4);
	}
	else
		printf("error from uart-pinmux \n");


	tranceiver = open("/dev/tty")



	FILE *pwmstate;
	FILE *pwmexp;
	char buf[22];
	char bufexp[5]="NULL,";
	//char buf[15];// /P9_14_pinmux.32 선언
	// slot에 잘 연결 안되면 segmentation fault 뜸
	char buf1[50] = "/sys/devices/ocp.3";
	char buf2[50] = "/sys/class/pwm/export";
	switch(expnum)
	{
	case 0 :
		strcpy(buf,"/P9_29_pinmux.44"); //for cape-universalk
		break;
	case 1 :
		strcpy(buf,"/P9_31_pinmux.46"); //for cape-universalk
		break;
	case 3 :
		strcpy(buf,"/P9_14_pinmux.33"); //for cape-universalk
		break;
	case 4 :
		strcpy(buf,"/P9_16_pinmux.35"); //for cape-universalk
		break;
	case 5 :
		strcpy(buf,"/P8_45_pinmux.28"); //for cape-universalk
		break;
	case 6 :
		strcpy(buf,"/P8_46_pinmux.29"); //for cape-universalk
		break;
	default :
		printf("Error!\n");
		break;
	}
	strcat(buf1,strcat(buf,"/state"));
		// /sys/devices/ocp.3 /P9_14_pinmux.32 /state
	pwmstate = fopen(buf1,"w");
	if(pwmstate == NULL) printf("Init_PWM failed to setting");
	fseek(pwmstate,0,SEEK_SET);
	fprintf(pwmstate,"pwm");
	printf("pwm > %s\n",buf1);
		//1:echo pwm > /sys/devices/ocp.3/P9_14_pinmux.32/state
	fflush(pwmstate);
	fclose(pwmstate);

	pwmexp = fopen(buf2,"w");
	if(pwmexp == NULL) printf("Init_PWM failed to export");
	sprintf(bufexp,"%i",expnum);
	fprintf(pwmexp,bufexp);
		//2:echo 3 > /sys/class/pwm/export
	fflush(pwmexp);
	fclose(pwmexp);

	return 1;
}

//Choose UART & Baudrate return file discripter and file pointer
//echo uart > /sys/devices/ocp.3/P9_26_pinmux.42/state
addr_RXD1 = fopen("/sys/devices/ocp.3/P9_26_pinmux.42/state","w");
if(addr_RXD1 == NULL) printf("RXD1 Pin fail to set\n");
fseek(addr_RXD1,0,SEEK_SET);
fprintf(addr_RXD1,"%s","uart");
fflush(addr_RXD1);
fclose(addr_RXD1);
printf("11st step\n");
RXD1 = fopen("/dev/ttyO1","r");	//if reading, "r", but R/W, "r+"
if(RXD1 == NULL) printf("RXD1 Port is not open!\n");
fseek(RXD1,0,SEEK_SET);
printf("12nd step\n");

//여기서는 다른 헤더의 명령어 세트를 사용했다.
RXD1i = open("/dev/ttyO1",O_RDWR | O_NOCTTY | O_NONBLOCK);
memset( &newtio, 0, sizeof(newtio) );  // 시리얼 포트 통신 환경 설정
newtio.c_cflag       = B115200 | CS8 | CLOCAL | CREAD;
newtio.c_iflag       = 0;              // NO_PARITY
newtio.c_oflag       = 0;
newtio.c_lflag       = 0;
newtio.c_cc[VTIME]   = 0;
newtio.c_cc[VMIN]    = 1;
tcflush(RXD1i,TCIFLUSH);		//RXD1를
tcsetattr(RXD1i, TCSANOW, &newtio); //즉시 적용한다.
fcntl(RXD1i, F_SETFL, FNDELAY);
//******************************************************//

//echo uart > /sys/devices/ocp.3/P9_22_pinmux.39/state
addr_RXD2 = fopen("/sys/devices/ocp.3/P9_22_pinmux.39/state","w");
if(addr_RXD2 == NULL) printf("RXD2 Pin fail to set\n");
fseek(addr_RXD2,0,SEEK_SET);
fprintf(addr_RXD2,"%s","uart");
fflush(addr_RXD2);
fclose(addr_RXD2);
printf("21st step\n");
RXD2 = fopen("/dev/ttyO2","r");	//if reading, "r", but R/W, "r+"
if(RXD2 == NULL) printf("RXD2 Port is not open!\n");
fseek(RXD2,0,SEEK_SET);
printf("22nd step\n");

//여기서는 다른 헤더의 명령어 세트를 사용했다.
RXD2i = open("/dev/ttyO2",O_RDWR | O_NOCTTY | O_NONBLOCK);
memset( &newtio, 0, sizeof(newtio) );  // 시리얼 포트 통신 환경 설정
newtio.c_cflag       = B115200 | CS8 | CLOCAL | CREAD;
newtio.c_iflag       = 0;              // NO_PARITY
newtio.c_oflag       = 0;
newtio.c_lflag       = 0;
newtio.c_cc[VTIME]   = 0;
newtio.c_cc[VMIN]    = 1;
tcflush(RXD2i,TCIFLUSH);		//RXD2를
tcsetattr(RXD2i, TCSANOW, &newtio); //즉시 적용한다.
fcntl(RXD2i, F_SETFL, FNDELAY);
//******************************************************//

//echo uart > /sys/devices/ocp.3/P9_22_pinmux.39/state
addr_RXD4 = fopen("/sys/devices/ocp.3/P9_11_pinmux.30/state","w");
if(addr_RXD4 == NULL) printf("RXD4 Pin fail to set\n");
fseek(addr_RXD4,0,SEEK_SET);
fprintf(addr_RXD4,"%s","uart");
fflush(addr_RXD4);
fclose(addr_RXD4);
printf("41st step\n");
RXD4 = fopen("/dev/ttyO4","r");	//if reading, "r", but R/W, "r+"
if(RXD4 == NULL) printf("RXD4 Port is not open!\n");
fseek(RXD4,0,SEEK_SET);
printf("42nd step\n");

//여기서는 다른 헤더의 명령어 세트를 사용했다.
RXD4i = open("/dev/ttyO4",O_RDWR | O_NOCTTY | O_NONBLOCK);
memset( &newtio, 0, sizeof(newtio) );  // 시리얼 포트 통신 환경 설정
newtio.c_cflag       = BAUDRATE | CS8 | CLOCAL | CREAD;
newtio.c_iflag       = 0;              // NO_PARITY
newtio.c_oflag       = 0;
newtio.c_lflag       = 0;
newtio.c_cc[VTIME]   = 0;
newtio.c_cc[VMIN]    = 1;
tcflush(RXD4i,TCIFLUSH);		//RXD4를
tcsetattr(RXD4i, TCSANOW, &newtio); //즉시 적용한다.
fcntl(RXD4i, F_SETFL, FNDELAY);




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
