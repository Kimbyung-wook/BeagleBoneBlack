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
	InitPin(23);
	SetPinDirt(23,OUT);
	SetPinValue(23,ON);

	printf("pwm0\n");
	InitPWM(1);
	SetPWMPeriod(1,20000000);
	  SetPWMDuty(1,10);
	SetPWMOnOff(1,ON);

	for(cnt = 0;cnt<8;cnt++)
	{
		SetPWMDuty(1,3.5+(double)cnt);
		usleep(500000);
	}

//**********************************************************//
//**********************************************************//
	printf("End\n");
	usleep(1000000);
	SetPinValue(23,OFF);
	CloseGPin(23);
	//SetPWMOnOff(3,OFF);
	//SetPWMOnOff(1,OFF);
	//ClosePWMPin(0);
	return 0;
}
