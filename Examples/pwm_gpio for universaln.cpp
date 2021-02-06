#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define MGRNUM 9
#define HELPER 14

#define OUT "out"
#define IN "in"
#define ON 1
#define OFF 0

//cape setting
int SettingCape(void);
//**********************************************************//
//GPIO Pin Setting
int InitPin(int PinNum);									//Pin Setting
int SetPinDirt(int PinNum, char* either);					//Pin Direction Setting
int SetPinValue(int PinNum, int Value);						//Pin Value Setting (On/Off)
int GetPinValue(int pinnum);								//Getting Pin Value
int CloseGPin(int PinNum);									//close pin
//**********************************************************//
//PWM pin Setting
int InitPWM(int expnum);
int SetPWMPeriod(int expnum,int period);
int SetPWMDuty(int expnum,int duty);
int SetPWMOnOff(int expnum, int run);
int ClosePWMPin(int PinNum);
//**********************************************************//
int main()
{
	int i=0,j=0;
	int cnt = 0;
	int gpio[6];
	gpio[0]=14;
	gpio[1]=15;
	gpio[2]=2;
	gpio[3]=3;
	gpio[4]=30;
	gpio[5]=31;
	printf("Start\n");
	SettingCape();
	printf("pwm0\n");
	InitPWM(0);
	SetPWMPeriod(0,500000000);
	   SetPWMDuty(0,50000000);
	SetPWMOnOff(0,ON);
	printf("pwm1\n");
	InitPWM(1);
	SetPWMPeriod(1,500000000);
	  SetPWMDuty(1,100000000);
	SetPWMOnOff(1,ON);
	printf("pwm3\n");
	InitPWM(3);
	SetPWMPeriod(3,500000000);
	  SetPWMDuty(3,200000000);
	SetPWMOnOff(3,ON);
	printf("pwm4\n");
	InitPWM(4);
	SetPWMPeriod(4,500000000);
	  SetPWMDuty(4,300000000);
	SetPWMOnOff(4,ON);
	printf("pwm5\n");
	InitPWM(5);
	SetPWMPeriod(5,500000000);
	  SetPWMDuty(5,400000000);
	SetPWMOnOff(5,ON);
	printf("pwm6\n");
	InitPWM(6);
	SetPWMPeriod(6,500000000);
	  SetPWMDuty(6,500000000);
	SetPWMOnOff(6,ON);

	for(cnt = 0;cnt<sizeof(gpio)/sizeof(int);cnt++)
	{
		InitPin(gpio[cnt]);
		SetPinDirt(gpio[cnt],OUT);
		SetPinValue(gpio[cnt],ON);
		printf("Pin %d : %d\n",gpio[cnt],cnt);
	}

//**********************************************************//
//**********************************************************//
	printf("End\n");
	usleep(10000000);
	for(cnt = 0;cnt<sizeof(gpio)/sizeof(int);cnt++)
	{
		SetPinValue(gpio[cnt],OFF);
		CloseGPin(gpio[cnt]);
	}/*
	SetPWMOnOff(0,OFF);
	SetPWMOnOff(1,OFF);
	SetPWMOnOff(3,OFF);
	SetPWMOnOff(4,OFF);
	SetPWMOnOff(5,OFF);
	SetPWMOnOff(6,OFF);
	ClosePWMPin(0);
	ClosePWMPin(1);
	ClosePWMPin(3);
	ClosePWMPin(4);
	ClosePWMPin(5);
	ClosePWMPin(6);*/
	return 0;
}
//**********************************************************
//*                      Cape Function                     *
//**********************************************************

int SettingCape(void)
{
	FILE * io;
	io = fopen("/sys/devices/bone_capemgr.9/slots","w");
	if(io == NULL) printf("This cape cant enter");
	fseek(io,0,SEEK_SET);
	fprintf(io,"%s","cape-universaln");
	fflush(io);
	fclose(io);
	return 0;
}
//**********************************************************
//*                      GPIO Function                     *
//**********************************************************
int InitPin(int PinNum)
{
	FILE *io;

	io = fopen("/sys/class/gpio/export","w");				//file open - gpio-export
	if(io == NULL) printf("Pin Failed to initialize_InitPin\n");	//if fail to open, print
	fseek(io,0,SEEK_SET);									//go FilePointer to first
	fprintf(io,"%d",PinNum);								//Open PinNum
	fflush(io);												//FilePointer clear
	fclose(io);												//File close

	return 0;
}

int SetPinDirt(int PinNum, char* either)					//'either' is direction (in or out)
{
	FILE *pdir;
	char buf[15];											//input "direction"
	char buf2[50] = "/sys/class/gpio/gpio";

	sprintf(buf,"%i",PinNum);								//duplicate PinNum to buf(memory)
	strcat(buf2,strcat(buf,"/direction"));					//make : buf2 + PinNum + "/direction"

	pdir = fopen(buf2,"w");									//open adress 'buf2' on writing condition
	if(pdir == NULL) printf("Direction failed to open\n");	//if fail to open, print
	fseek(pdir,0,SEEK_SET);									//go FilePointer to first
	fprintf(pdir,"%s",either);								//printf 'either' to filepointer 'pdir'
	fflush(pdir);
	fclose(pdir);

	return 0;
}

int SetPinValue(int PinNum, int Value)
{
	FILE *val;
	char buf[10];											//input "value"
	char buf2[50] = "/sys/class/gpio/gpio";

	sprintf(buf,"%i",PinNum);								//duplicate PinNum to buf(memory)
	strcat(buf2,strcat(buf,"/value"));						//make : buf2 + PinNum + "/value"

	val = fopen(buf2,"w");
	if(val == NULL) printf("Value failed to open\n");
	fseek(val,0,SEEK_SET);
	fprintf(val,"%d",Value);
	fflush(val);
	fclose(val);

	return 0;
}
int GetPinValue(int pinnum)
{
	FILE *val;
	int value;
	char buf[5];
	char buf2[50] = "/sys/class/gpio/gpio";

	//build file path to value file
	sprintf(buf,"%i",pinnum);
	strcat(buf2,strcat(buf,"/value"));

	val = fopen(buf2, "r");
	if(val == NULL) printf("Input value failed to open\n");
	fseek(val,0,SEEK_SET);
	fscanf(val,"%d",&value);
	fclose(val);

	return value;
}

int CloseGPin(int PinNum)
{
	FILE *io;

	io = fopen("/sys/class/gpio/unexport","w");
	if(io == NULL) printf("Pin Failed to initialize_ClosePin\n");
	fseek(io,0,SEEK_SET);
	fprintf(io,"%d",PinNum);
	fflush(io);
	fclose(io);

	return 0;
}

//*************************************************
//*                PWM FUNCTIONS                  *
//*************************************************
int InitPWM(int expnum)
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
	FILE *pwmstate;
	FILE *pwmexp;
	char buf[22];
	char bufexp[5]="NULL,";
	//char buf[15];// /P9_14_pinmux.32 선언
	//나중에는 slot에 제대로 들어갔는지 확인하는 구문이 있어야함
	char buf1[50] = "/sys/devices/ocp.3";
	char buf2[50] = "/sys/class/pwm/export";
	switch(expnum)
	{
	case 0 :
		//strcpy(buf,"/P9_22_pinmux.38");
		strcpy(buf,"/P9_29_pinmux.38");
		break;
	case 1 :
		//strcpy(buf,"/P9_21_pinmux.37");
		break;
	case 3 :
		strcpy(buf,"/P9_14_pinmux.32");
		break;
	case 4 :
		strcpy(buf,"/P9_16_pinmux.34");
		break;
	case 5 :
		strcpy(buf,"/P8_19_pinmux.27");
		break;
	case 6 :
		strcpy(buf,"/P8_13_pinmux.21");
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
	//1:echo pwm > /sys/devices/ocp.3/P9_16_pinmux.34/state
	fflush(pwmstate);
	fclose(pwmstate);

	pwmexp = fopen(buf2,"w");
	if(pwmexp == NULL) printf("Init_PWM failed to export");
	sprintf(bufexp,"%i",expnum);
	fprintf(pwmexp,bufexp);
	//2:echo 3 > /sys/class/pwm/export
	//2:echo 4 > /sys/class/pwm/export
	fflush(pwmexp);
	fclose(pwmexp);

	return 1;
}

int SetPWMPeriod(int expnum,int period)
{
	FILE *pwm;
	char buf[15];
	char buf2[60] = "/sys/class/pwm/pwm";
	//3:echo 1000000000 > /sys/class/pwm/pwm3/period_ns

	sprintf(buf,"%i",expnum);
	strcat(strcat(buf2,buf),"/period_ns");
	// /sys/class/pwm/pwm3/period_ns

	pwm = fopen(buf2, "w");
	if(pwm == NULL) printf("PWM Period failed to open\n");
	fseek(pwm,0,SEEK_SET);
	fprintf(pwm,"%d",period);
	printf("%d > %s\n",period,buf2);
	fflush(pwm);
	fclose(pwm);

	return 0;
}
int SetPWMDuty(int expnum,int duty)
{
	FILE *pwm;
	char buf[15];
	char buf2[60] = "/sys/class/pwm/pwm";
	//4:echo 500000000 > /sys/class/pwm/pwm3/duty_ns

	sprintf(buf,"%i",expnum);
	strcat(strcat(buf2,buf),"/duty_ns");

	// /sys/class/pwm/pwm3/duty_ns
	pwm = fopen(buf2, "w");
	if(pwm == NULL) printf("PWM Duty failed to open\n");
	fseek(pwm,0,SEEK_SET);
	fprintf(pwm,"%d",duty);
	printf("%d > %s\n",duty,buf2);
	fflush(pwm);
	fclose(pwm);

	return 0;
}

int SetPWMOnOff(int expnum, int run)
{
	FILE *pwm;
	char buf[5];
	char buf2[50] = "/sys/class/pwm/pwm";
	sprintf(buf,"%i",expnum);
	strcat(strcat(buf2,buf),"/run");

	pwm = fopen(buf2, "w");
	if(pwm == NULL) printf("PWM Run failed to open\n");
	fseek(pwm,0,SEEK_SET);
	fprintf(pwm,"%d",run);
	printf("%d > %s\n",run,buf2);
	fflush(pwm);
	fclose(pwm);

	return 0;
}

int ClosePWMPin(int PinNum)
{
	FILE *io;

	io = fopen("/sys/class/pwm/unexport","w");
	if(io == NULL) printf("PWM Failed to initialize_ClosePWMPin\n");
	fseek(io,0,SEEK_SET);
	fprintf(io,"%d",PinNum);
	fflush(io);
	fclose(io);

	return 0;
}
