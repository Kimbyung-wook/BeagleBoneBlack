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
	int i=0,j=0;
	int cnt = 0;
	printf("Start\n");
	SettingCape();
	LEDOn(23);
	LEDOn(47);

	printf("pwm0_On\n");
	InitPWM(0);
	SetPWMPeriod(0,20000000);
	SetPWMDuty(0,11.0);
	SetPWMOnOff(0,ON);

	printf("pwm1_On\n");
	InitPWM(1);
	SetPWMPeriod(1,20000000);
	SetPWMDuty(1,4);
	SetPWMOnOff(1,ON);

	printf("Select 0 or 1\n");
	usleep(1000000);

	for(i=0;i<5;i++)
	{
		SetPinValue(23,OFF);
		printf("Select 0\n");
		usleep(1000000);

		SetPinValue(23,ON);
		printf("Select 1\n");
		usleep(1000000);
	}

	printf("End\n");


//**********************************************************//

	LEDOff(46);
	LEDOff(23);
	//ClosePWMPin(0);
	//ClosePWMPin(1);

	return 0;
}
