/*
 * muxboard128.c
 *
 * Created: 2015-12-05 오전 2:47:52
 * Author: Cfp
 */
#include <mega128.h>
#include <stdio.h>
#include <delay.h>
#include <string.h>
volatile unsigned int cnt_duty=0;
int NotPrint = 0;


void uart_init()         // UART0 - baudrate : 115200 bps
{
    delay_ms(10);
    DDRE |= 0b00000010;
    UCSR0A=0;
    UCSR0B=0b00001000;  //송신기 설정
    //UCSR0B=0b10011000;  //송수신기 설정
    UCSR0C=0b00000110;  //8bits
    UBRR0H=0;
    UBRR0L=8;         //Boudrates 115200bps
                      //Baudrate 9600 - 108? 일때
}
void timer_init()        // Fast PWM 14 mode, 8 scaler, 0.5us count.
{
    TCCR3A = 0b00000010;
    TCCR3B = 0b00011010;
    ICR3 = 39999;       // 20 msec period.
    TCCR1A = 0b10100010;
    TCCR1B = 0b00011010;
    ICR1 = 39999;
    OCR1A = 0;
    OCR1B = 0;
}
void port_init()
{
    DDRF = 0b10000000;      // MUX selecter PF7
    PORTF = 0b00000000;     // 
    DDRE = 0b00000000;      // INT 4,5 수신방향으로 설정
    PORTE = 0b00110000;     // INT4,5 내부 풀업저항 사용
    DDRD = 0b00001100;      // RXD1 TXD1 OUT
    PORTD = 0b00001100;
    
    EIMSK = 0b00110000;    // INT4, 5 외부인터럽트 개별 허용
    EICRB = 0b00001111;    // INT4, 5 - rising edge 트리거 설정
}
interrupt [EXT_INT4] void ext_int4(void)     // 인터럽트 발생 순서3
{
    if((EICRB&0b00000001)==1){
        TCNT1 = 0;
        EICRB -= 0b00000001;
    }
    else{
        cnt_duty = TCNT1;
        NotPrint = 1;
        EICRB += 0b00000001;
    }
}
void main(void)
{
    port_init();
    uart_init();
    timer_init();
    
    SREG |= 0x80;    // 전체 인터럽트 허용

    printf("Hello");    
while (1)
    {
        if(NotPrint == 1)
        {                                    
            printf("duty : %d\n",cnt_duty);
            if(cnt_duty>3500)
            {
                PORTF = 0b10000000;
                PORTD = 0b00001100;
                printf("1");
            }else
            {
                PORTF = 0b00000000;
                PORTD = 0b00000000;
                printf("2");
            }
            NotPrint = 0;
        }
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
        if(Flag == 0b00000011)
        {
            Servo_duty[0] = GetLine[Linecomma[0]+1];
            Servo_duty[1] = GetLine[Linecomma[1]+1];
            PORTA = Servo_duty[Select];
            OCR1A = (Servo_duty[Select]-125)*((ICR1+1)/2500)+3000;
            printf("%d,%s,0:%d,1:%d\n",Servo_duty[Select],GetLine,Linecomma[0],Linecomma[1]);
            Flag = 0;     // Last character 발견 취소, 다시 읽어
            Linecnt = 0;    // 처음부터 다시 읽기
            StartLine = 0;
            
        }
        else
            continue;
    }
}
