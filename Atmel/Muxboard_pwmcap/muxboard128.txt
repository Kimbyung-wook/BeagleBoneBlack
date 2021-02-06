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
    TCCR1A = 0b00000010;
    TCCR1B = 0b00011010;
    ICR1 = 40000;       // 20 msec period.
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
    if((EICRB&0b00000001)==1){				// At rising edge,
        TCNT1 = 0;							// Counter initializing
        EICRB -= 0b00000001;				// for falling edge
    }
    else{
        cnt_duty = TCNT1;					// write duty
        NotPrint = 1;
        EICRB += 0b00000001;				// for rising edge
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
        
    }
}
