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
	int AHRS_ID = 0;
	double AHRS_phi = 0;
	double AHRS_theta = 0;
	double AHRS_psi = 0;
	double AHRS_theta_b=0;
	double AHRS_theta_f=0;
	double AHRS_f_tau = 0.1;
	double PID_CMDtheta=0;
	double PID_P=100;
	double PID_I=0;
	double PID_D=100;
	double PID_CMDduty=0;
	double PID_Integral=0;
	double time_del = 0.001;

	int i1,j1;
	int i2,j2;
	int i4,j4;
	int UART1_News=1;
	int UART2_News=1;
	int UART4_News=1;
	int readfromUART1 = 0;
	int readfromUART2 = 0;
	int readfromUART4 = 0;
	int UART1T=0;
	int UART1R=0;
	int UART2T=0;
	int UART2R=0;
	int UART4T=0;
	int UART4R=0;
	char UART1_buf[100] = {'\0',};
	char UART2_buf[100] = {'\0',};
	char UART4_buf[100] = {'\0',};
	char UART1_data[500] = {'\0',}; //AHRS1
	char UART2_data[500] = {'\0',}; //AHRS2
	char UART4_data[500] = {'\0',}; //AHRS2
	char UART1_forParse[100] = {'\0',};
	char UART2_forParse[100] = {'\0',};
	char UART4_forParse[100] = {'\0',};
	int UART1_position[3] = {0,0,0};
	int UART2_position[3] = {0,0,0};
	int UART4_position[3] = {0,0,0};
	// UART1_position[0] => $ position
	// UART1_position[1] => * position
	// UART1_position[2] => before * position
	int UART1_cpy_flag = 0;
	int UART2_cpy_flag = 0;
	int UART4_cpy_flag = 0;
	char test1[50]={'\0',};
	char test2[50]={'\0',};
	char test4[50]={'\0',};

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

	UART2T=UART_init(2,'T');
	printf("UART2T : %d\n",UART2T);
	UART2R=UART_init(2,'R');
	printf("UART2R : %d\n",UART2R);
	UART_Baudrate_NonC(UART2T,115200,0,1);

	UART4T=UART_init(4,'T');
	printf("UART4T : %d\n",UART4T);
	UART4R=UART_init(4,'R');
	printf("UART4R : %d\n",UART4R);
	UART_Baudrate_NonC(UART4T,115200,0,1);

	printf("START!\n");
	write(UART2T,"START!\n",8);
	while(!GetPinValue(27))
	{
		//AHRS
		readfromUART1 = read(UART1R,UART1_buf,100);	//받았으면
		strcat(UART1_data,UART1_buf);				//복사하고
		Nullize(UART1_buf,readfromUART1);			//초기화
/*
		//GPS
		readfromUART2 = read(UART2R,UART2_buf,100);	//받았으면
		strcat(UART2_data,UART2_buf);				//복사하고
		Nullize(UART2_buf,readfromUART2);			//초기화

		//RF
		readfromUART4 = read(UART4R,UART4_buf,100);	//받았으면
		strcat(UART4_data,UART4_buf);				//복사하고
		Nullize(UART4_buf,readfromUART4);			//초기화
*/
		if(readfromUART1!=-1)
		{
			//write(UART1T,UART1_data,strlen(UART1_data)+1);
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
			//sprintf(test1,"\nCopy! flag : %d, 0-%i, 1-%i, %i\n",UART1_cpy_flag,UART1_position[0],UART1_position[1],(UART1_cpy_flag==2)&&(j1+UART1_position[0]<UART1_position[1]));
			//write(UART1T,test1,strlen(test1)+1);
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
			//sprintf(test1,"Copy for parse %i - posi2 %i\n",UART1_cpy_flag,UART1_position[2]);
			//write(UART1T,test1,strlen(test1)+1);
			if(UART1_cpy_flag==3)
			{
				sprintf(test1,"1|%s|\n",UART1_forParse);
				//write(UART1T,test1,strlen(test1)+1);
				AHRS_theta = AHRS_theta_b; // before theta
				AHRS_Parsing_RPY5(UART1_forParse,&AHRS_ID,&AHRS_phi,&AHRS_theta,&AHRS_psi);
				if(AHRS_theta_f==0)
					AHRS_theta_f=AHRS_theta;
				AHRS_theta_f = (AHRS_f_tau/(AHRS_f_tau+time_del))*AHRS_theta_f +(time_del/(AHRS_f_tau+time_del))*AHRS_theta;
				printf("theta : %10.3f,\t Filtered theta : %10.3f\n",AHRS_theta,AHRS_theta_f);
				sprintf(test1,"theta : %10.3f,\t Filtered theta : %10.3f\n",AHRS_theta,AHRS_theta_f);
				write(UART2T,test1,strlen(test1)+1);
				Nullize(UART1_forParse,sizeof(UART1_forParse));
				if(strlen(UART1_data)>300)
				{
					Nullize(UART1_data,strlen(UART1_data));
					UART1_position[0]=0;
					UART1_position[1]=0;
					UART1_position[2]=0;
				}
			}
		}/*
		if(readfromUART2!=-1)
		{
			for(i2=UART2_position[2],UART2_position[0]=UART2_position[2],UART2_position[1]=UART2_position[2]+1,UART2_cpy_flag=0;i2<sizeof(UART2_data);i2++)		//데이터를 처음부터 읽어온다
			{
				// $ 발견하면 위치를 기록(flag=1), *가 발견되면 위치를 기록(flag=2)하고 탈출한다.
				// *가 발견되지 않을 경우, $만 발견했을 경우에 (flag=1) 기록을 취소하고 다시 read
				if(UART2_data[i2]=='$')				//Find $
				{
					UART2_position[0]=i2;
					UART2_cpy_flag = 1;				//Detect $
					i2++;
				}
				//Find End
				if((UART2_cpy_flag==1)&&(UART2_data[i2-3]=='*'))
				{
					UART2_position[1]=i2;
					UART2_cpy_flag = 2;				//Detect $
					break;//복사 끝났다 나가자
				}
				if((UART2_data[i2]=='\0')&&(UART2_cpy_flag==1))
				{//문자열의 마지막이면서 아직 *를 찾지 못했다면
					UART2_position[0]=UART2_position[2];
					UART2_position[1]=UART2_position[2]+1;
					break;
				}
			} // End to search string
			//검사 끝
			for(j2=0;(UART2_cpy_flag==2)&&(j2+UART2_position[0]<UART2_position[1]);j2++)
			{
			// Detect * && smaller than '*' position
				UART2_forParse[j2] = UART2_data[UART2_position[0]+j2];
				if(UART2_forParse[j2-2]=='*')
				{
					UART2_forParse[j2+1]='\0';
					UART2_cpy_flag=3;
					UART2_position[2] = UART2_position[1];
					break;
				}
			}
			if(UART2_cpy_flag==3)
			{
				sprintf(test2,"2|%s|\n",UART2_forParse);
				printf("%s",test2);
				Nullize(UART2_forParse,sizeof(UART2_forParse));
				if(strlen(UART2_data)>300)
				{
					Nullize(UART2_data,strlen(UART2_data));
					UART2_position[0]=0;
					UART2_position[1]=0;
					UART2_position[2]=0;
				}
			}
		}
		if(readfromUART4!=-1)
		{
			write(UART4T,UART4_data,strlen(UART4_data)+1);
			for(i4=UART4_position[2],UART4_position[0]=UART4_position[2],UART4_position[1]=UART4_position[2]+1,UART4_cpy_flag=0;i4<sizeof(UART4_data);i4++)		//데이터를 처음부터 읽어온다
			{
				// $ 발견하면 위치를 기록(flag=1), *가 발견되면 위치를 기록(flag=2)하고 탈출한다.
				// *가 발견되지 않을 경우, $만 발견했을 경우에 (flag=1) 기록을 취소하고 다시 read
				if(UART4_data[i4]=='$')				//Find $
				{
					UART4_position[0]=i4;
					UART4_cpy_flag = 1;				//Detect $
					i4++;
				}
				//Find End
				if((UART4_cpy_flag==1)&&(UART4_data[i4-3]=='*'))
				{
					UART4_position[1]=i4;
					UART4_cpy_flag = 2;				//Detect $
					break;//복사 끝났다 나가자
				}
				if((UART4_data[i4]=='\0')&&(UART4_cpy_flag==1))
				{//문자열의 마지막이면서 아직 *를 찾지 못했다면
					UART4_position[0]=UART4_position[2];
					UART4_position[1]=UART4_position[2]+1;
					break;
				}
			} // End to search string
			//검사 끝
			sprintf(test1,"\nCopy! flag : %d, 0-%i, 1-%i, %i\n",UART4_cpy_flag,UART4_position[0],UART4_position[1],(UART4_cpy_flag==2)&&(j1+UART4_position[0]<UART4_position[1]));
			write(UART4T,test1,strlen(test1)+1);
			for(j4=0;(UART4_cpy_flag==2)&&(j4+UART4_position[0]<UART4_position[1]);j4++)
			{
			// Detect * && smaller than '*' position
				UART4_forParse[j4] = UART4_data[UART4_position[0]+j4];
				if(UART4_forParse[j4-2]=='*')
				{
					UART4_forParse[j4+1]='\0';
					UART4_cpy_flag=3;
					UART4_position[2] = UART4_position[1];
					break;
				}
			}
			if(UART4_cpy_flag==3)
			{
				sprintf(test4,"4-%s-\n",UART4_forParse);
				write(UART4T,test4,strlen(test1)+1);
				printf("%s",test4);
				Nullize(UART4_forParse,sizeof(UART4_forParse));
				if(strlen(UART4_data)>300)
				{
					Nullize(UART4_data,strlen(UART4_data));
					UART4_position[0]=0;
					UART4_position[1]=0;
					UART4_position[2]=0;
				}
			}
		}*/
	}
	//UART_Close(1,'T');
	//UART_Close(1,'R');
	write(UART4T,"End\n",5);
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
