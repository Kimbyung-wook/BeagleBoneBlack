#include <stdio.h>


#define Deviceaddr "/sys/devices/ocp.3/"
#define UARTaddr "/dev/ttyO"

//UART Prototypes
/*int initUART(int mgrnum, char* uartnum);
void closeUART(int fd);
int configUART(UART u, int property, char* value);
int txUART(int uart, unsigned char data);
unsigned char rxUART(int uart);
int strUART(int uart, char* buf);*/

int main()
{
    FILE *slot;
    FILE *TX //9_24 UART1 TX
    FILE *RX //9_26 UART1 RX
    FILE *ttyO0 //ttyO0 address
    FILE *ttyO1 //ttyO1 address
    
    char sentence[40] = "Hello world!\n";
    char buf[50] = "NULL,";
    char buf_p40[50] = "P9_24_pinmux.40"; //TX
    char buf_p41[50] = "P9_26_pinmux.41"; //RX
    char buf_txfile[50] = "NULL,";        //TX
    char buf_rxfile[50] = "NULL,";        //RX
    char buf_txaddr[50] = "NULL,";        //TX
    char buf_rxaddr[50] = "NULL,";        //RX
    
    
    
    //**********************************************************//
    //TX commend
    //echo uart > /sys/devices/ocp.3/P9_24_pinmux.40/state
    strcat(buf_txfile,Deviceaddr);
    strcat(buf_txfile,strcat(buf_p40,"/state"));
    printf("%s\n",buf_txfile);
    TX = fopen("buf_tx","w");
    if(TX = NULL) printf("TX file failed to open");
    fseek(TX,0,SEEK_SET);
    fprintf(TX,"%s","uart");                                      //Define UART
    //fprintf(TX,"%s","default");                                   //Freeing port
    fflush(TX);
    fclose(TX);
    //**********************************************************//
    //RX commend
    //echo uart > /sys/devices/ocp.3/P9_26_pinmux.41/state
    strcat(buf_rx,Deviceaddr);
    strcat(buf_rx,strcat(buf_p41,"/state"));
    printf("%s\n",buf_rxfile);
    RX = fopen("buf_rx","w");
    if(RX = NULL) printf("RX file failed to open");
    fseek(RX,0,SEEK_SET);
    fprintf(RX,"%s","uart");                                      //Define UART
    //fprintf(RX,"%s","default");                                   //Freeing port
    fflush(RX);
    fclose(RX);
    
    //
    //echo hi > /dev/tty01
    strcat(buf_txaddr,UARTaddr);
    strcat(buf_txaddr,"1");
    printf("%s\n",buf_txfile);
    TX = fopen("buf_tx","w");
    if(TX = NULL) printf("TX send failed to open");
    fseek(TX,0,SEEK_SET);
    fprintf(TX,"%s","hi");
    fflush(TX);
    fclose(TX);
    
    
    //echo hello > /dev/tty01
    
    
    
    
