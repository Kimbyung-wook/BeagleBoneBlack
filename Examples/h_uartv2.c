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
void AHRS_Parsing_RPY5(char *Data,int *ahrs_num1,double *phi,double *theta,double *psi);

int main()
{
	int i1,j1,k1,l1;
	int i2,j2,k2,l2;
	int cnt;
	int UART1_News=1;
	int UART2_News=1;
	int UART3_News=1;
	int readfromUART1 = 0;
	int readfromUART2 = 0;
	int readfromUART3 = 0;
	int UART1T=0;
	int UART1R=0;
	int UART2T=0;
	int UART2R=0;
	int UART3T=0;
	int UART3R=0;
	char UART1_buf[100] = {'\0',};
	char UART2_buf[100] = {'\0',};
	char UART3_buf[100] = {'\0',};
	char UART1_data[500] = {'\0',}; //AHRS1
	char UART2_data[500] = {'\0',}; //AHRS2
	char UART3_data[500] = {'\0',}; //AHRS2
	char UART1_forParse[100] = {'\0',};
	char UART2_forParse[100] = {'\0',};
	char UART3_forParse[100] = {'\0',};
	int UART1_position[3] = {0,0,0};
	int UART2_position[3] = {0,0,0};
	// UART1_position[0] => $ position
	// UART1_position[1] => * position
	// UART1_position[2] => before * position
	int UART1_cpy_flag = 0;
	int UART2_cpy_flag = 0;
	int UART3_cpy_flag = 0;
	char test1[50]={'\0',};
	char test2[50]={'\0',};

	SettingCape();
	LEDOn(23);
	LEDOn(47);
	InitPin(27);
	SetPinDirt(27,IN);

	printf("Start\n");

	UART1T=UART_init(1,'T');
	printf("UART1T : %d\n",UART1T);
	UART1R=UART_init(1,'R');
	printf("UART1R : %d\n",UART1R);
	UART_Baudrate_NonC(UART1T,115200,0,1);

	printf("START!\n");
	write(UART1T,"START!\n",8);
	while(!GetPinValue(27))
	{
		readfromUART1 = read(UART1R,UART1_buf,100);	//받았으면
		strcat(UART1_data,UART1_buf);				//복사하고
		Nullize(UART1_buf,readfromUART1);			//초기화

		//if(((readfromUART1!=-1)&&(readfromUART2!=-1))&&(readfromUART4!=-1))		//정보 수신중에는 다른짓 못하게 해라
		if(readfromUART1!=-1)
		{
			write(UART1T,UART1_data,strlen(UART1_data)+1);
			for(i1=UART1_position[2],UART1_position[0]=UART1_position[2],UART1_position[1]=UART1_position[2]+1,UART1_cpy_flag=0;i1<sizeof(UART1_data);i1++)		//데이터를 처음부터 읽어온다
			{
				// $ 발견하면 위치를 기록(flag=1), *가 발견되면 위치를 기록(flag=2)하고 탈출한다.
				// *가 발견되지 않을 경우, $만 발견했을 경우에 (flag=1) 기록을 취소하고 다시 read
				if(UART1_data[i1]=='$')				//Find $
				{
					UART1_position[0]=i1;
					UART1_cpy_flag = 1;				//Detect $
					i1++;
				}
				//Find End
				if((UART1_cpy_flag==1)&&(UART1_data[i1-3]=='*'))
				{
					UART1_position[1]=i1;
					UART1_cpy_flag = 2;				//Detect $
					break;//복사 끝났다 나가자
				}
				if((UART1_data[i1]=='\0')&&(UART1_cpy_flag==1))
				{//문자열의 마지막이면서 아직 *를 찾지 못했다면
					UART1_position[0]=UART1_position[2];
					UART1_position[1]=UART1_position[2]+1;
					break;
				}
			} // End to search string
			//검사 끝
			sprintf(test1,"\nCopy! flag : %d, 0-%i, 1-%i, %i\n",UART1_cpy_flag,UART1_position[0],UART1_position[1],(UART1_cpy_flag==2)&&(j1+UART1_position[0]<UART1_position[1]));
			write(UART1T,test1,strlen(test1)+1);
			for(j1=0;(UART1_cpy_flag==2)&&(j1+UART1_position[0]<UART1_position[1]);j1++)
			{
			// Detect * && smaller than '*' position
				UART1_forParse[j1] = UART1_data[UART1_position[0]+j1];
				if(UART1_forParse[j1-2]=='*')
				{
					UART1_forParse[j1+1]='\0';
					UART1_cpy_flag=3;
					UART1_position[2] = UART1_position[1];
					break;
				}
			}
			sprintf(test1,"Copy for parse %i - posi2 %i\n",UART1_cpy_flag,UART1_position[2]);
			write(UART1T,test1,strlen(test1)+1);

			if(UART1_cpy_flag==3)
			{
				sprintf(test1,"1%s\n",UART1_forParse);
				//write(UART1T,test1,strlen(test1)+1);
				printf("%s",test1);
				Nullize(UART1_forParse,sizeof(UART1_forParse));
				if(strlen(UART1_data)>300)
				{
					Nullize(UART1_data,strlen(UART1_data));
					UART1_position[0]=0;
					UART1_position[1]=0;
					UART1_position[2]=0;
				}
			}
		}
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
void AHRS_Parsing_RPY5(char *Data,int *ahrs_num1,double *phi,double *theta,double *psi)
{
	int i,j,k,token_cnt=0;
	int comm_check=0;
	char temp[10]="'\0',";
	//char AHRS_datatype[10]="'\0',";
	char AHRS_ID[10]="'\0',";
	char AHRS_phi[10]="'\0',";
	char AHRS_theta[10]="'\0',";
	char AHRS_psi[10]="'\0',";
	//char AHRS_chksum[10]="'\0',";
	//$RPY , 41 , 179.60 , 36.26 , 13.91 * 66
	for(i=0,token_cnt=0;i<strlen(Data);i++)
	{
		for(j=0;((Data[i+j]!=',')&&(Data[i+j]!='*'));j++)
		{
			temp[j]=Data[i+j];
			//printf("char : %c\n",temp[j]);
			if(temp[j] == '\0')
			{
				break;
			}
		}//콤마사이에 값을 문자배열로 받아옴
		temp[j]= '\0';
		for(k=0;k<strlen(temp);k++)
		{
			if(temp[k]=='.')
				comm_check=1;
			if(comm_check==1)
				temp[k]=temp[k+1];
		}
		comm_check=0;
		switch(token_cnt)
		{
			/*case 0 :
				strcpy(AHRS_datatype,temp);
				printf("Type : %s\n",AHRS_datatype);
				break;*/
			case 1 :
				strcpy(AHRS_ID,temp);
				*ahrs_num1=atoi(AHRS_ID);
				//printf("ID : %d\n",atoi(AHRS_ID));
				break;
			case 2 :
				strcpy(AHRS_phi,temp);
				*phi=((double)atoi(AHRS_phi))/100;
				//printf("Phi : %f\n",atof(AHRS_phi));
				break;
			case 3 :
				strcpy(AHRS_theta,temp);
				*theta=((double)atoi(AHRS_theta))/100;
				//printf("Theta : %i,%s\n",atoi(AHRS_theta),AHRS_theta);
				break;
			case 4 :
				strcpy(AHRS_psi,temp);
				*psi=((double)atoi(AHRS_psi))/100;
				//printf("Psi : %f\n",atof(AHRS_psi));
				break;
			/*case 5 :
				break;*/
		}
		token_cnt++;
		i+=j;
	}
}
