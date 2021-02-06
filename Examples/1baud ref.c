/*
 * baud.c
 *
 *  Created on: 2015. 10. 6.
 *      Author: home
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <termios.h>                   // B115200, CS8 �� ��� ����
#include <fcntl.h>                     // O_RDWR , O_NOCTTY ���� ��� ����




int main( void)
{
   int    fd;
   int    fd1=open("/sys/devices/bone_capemgr.9/slots",O_WRONLY|S_IWRITE);;
   int    ndx;
   int    cnt;
   char   buf[1024];
   struct termios    newtio;
   struct pollfd     poll_events;      // üũ�� event ������ ���� struct
   int    poll_state;

    if ( 0 > fd1 )
    {
       printf("Capemgr error error\n");
       return -1;
    }
    write(fd1,"cape-universaln",16);
	printf("can use cape-universaln \n");




   // �ø��� ��Ʈ�� open

   fd = open( "/dev/ttyO2", O_RDWR | O_NOCTTY | O_NONBLOCK );        // ����̽��� open �Ѵ�.
   if ( 0 > fd)
   {
      printf("open error\n");
      return -1;
   }

   // �ø��� ��Ʈ ��� ȯ�� ����

   memset( &newtio, 0, sizeof(newtio) );
   newtio.c_cflag       = B115200 | CS8 | CLOCAL | CREAD;
   newtio.c_oflag       = 0;
   newtio.c_lflag       = 0;
   newtio.c_cc[VTIME]   = 0;
   newtio.c_cc[VMIN]    = 1;

   tcflush  (fd, TCIFLUSH );
   tcsetattr(fd, TCSANOW, &newtio );
   fcntl(fd, F_SETFL, FNDELAY);

}
