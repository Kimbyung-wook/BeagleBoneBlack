//Intro
/*
 * ActuatorBoard_ver1.c
 *
 * Created: 2015-12-30 ���� 5:14:15
 * Author: David Kim
 */
#include <mega128.h>
#include <stdio.h>
#include <delay.h>

// Define global variable
unsigned char GetLine[20] = {'\0',};           // ���� ���� ����
unsigned char Flag = 0;            // ���ڿ� flag
unsigned char Linecnt = 0;            // ��ġ ������
int Linecomma[4] = {0,};

interrupt [USART0_RXC] void usart0_rx_isr(void)
{
    GetLine[Linecnt++] = UDR0;
}

void main(void)
{
// Define local variable
    unsigned char Servo_duty[2] = {'\0',};
    int i=0;
    int j=0;
    unsigned char StartLine = 0;
    int Select = 0;
    
// Function : Bit7=In Bit6=Out Bit5=Out Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
    DDRB = 0b01100000;
// Identifier of duty signal 
    DDRA = 0b11111111;

// Timer/Counter 1 initialization
/* Prescaler : 8
// Mode: Fast PWM top=ICR1
// OC1A output: Non-Inverted PWM
// OC1B output: Non-Inverted PWM
// Timer Period: 20 ms
// Output Pulse(s):
// OC1A Period: 20 ms
// OC1B Period: 20 ms  */
    TCCR1A = 0b10100010;
    TCCR1B = 0b00011010;
    ICR1 = 39999;
    OCR1A = 0;
    OCR1B = 0;
    
// Timer(s)/Counter(s) Interrupt(s) initialization   
    TIMSK = 0b00000000;
    
// USART0 initialization
/* Communication Parameters: 8 Data, 1 Stop, No Parity
// USART0 Receiver: On
// USART0 Transmitter: Off
// USART0 Mode: Asynchronous
// USART0 Baud Rate: 38400  */ 
    UCSR0A = 0b00000000;
    UCSR0B = 0b10011000;
    UCSR0C = 0b00000110;
    UBRR0H=0x00;
    UBRR0L=0x19;
    
// Global enable interrupts
#asm("sei")


    PORTA = 0xFF;
    UDR0 = '*';
    delay_ms(500);
    PORTA = 0;
    UDR0 = '*';
    delay_ms(500);
        
while (1)
    {
        // �ڵ� ���� �� ��� ��������
        /*
         �Ʒ��� ���� �ڵ� ������
         ��� �ֱ� ���� ������ �ѹ� �̻� �� ���� �����ϰ� �ۼ���
         Comm. Protocol ( - �� char �ڷ�����)
         <-,->
         < : First Character
         , : seperator
         > : Last Character  
        �ڷ����� ���� 0 ~ 250 ���̷� �ԷµǸ�
        �̷� ���� ��Ƽ�� 10~20%�� �����δ�. 
        */
        for(i=0,j=1;i<=Linecnt;i++)
        {
            if(GetLine[i] == '<')
                Flag = Flag | 0b00000001;
            if(GetLine[i] == ',')
                Linecomma[j++]=i;
            if(GetLine[i] == '>')
            {
                Flag = Flag | 0b00000010;
                GetLine[i+1] = '\0';
            }
        }
        /*UDR0 = '\n';
        delay_ms(10);
        UDR0 = '-';
        UDR0 = Flag + 48;
        delay_ms(10);
        UDR0 = '-';
        UDR0 = Linecnt + 48;
        delay_ms(10);
        UDR0 = '-';
        delay_ms(10);
        UDR0 = '\n';*/
        if(Flag == 0b00000011)
        {
            Servo_duty[0] = GetLine[Linecomma[0]+1];
            Servo_duty[1] = GetLine[Linecomma[1]+1];
            PORTA = Servo_duty[Select];
            OCR1A = (Servo_duty[Select]-125)*((ICR1+1)/2500)+3000;
            printf("%d,%s,0:%d,1:%d\n",Servo_duty[Select],GetLine,Linecomma[0],Linecomma[1]);
            Flag = 0;     // Last character �߰� ���, �ٽ� �о�
            Linecnt = 0;    // ó������ �ٽ� �б�
            StartLine = 0;
            
        }
        else
            continue;
    }
}