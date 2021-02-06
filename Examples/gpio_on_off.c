//http://cafe.naver.com/nixtek1/589

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

/* GPIO registers */
#define GPIO0_BASE_START	0x44e07000	//AM335x GPIO0 base
#define GPIO0_BASE_END		0x44e07fff	//AM335x GPIO0 base
#define GPIO_SIZE			GPIO0_BASE_END - GPIO0_BASE_START
#define GPIO_DATAOUT		0x0000013c
#define GPIO_OE				0x00000134
#define GPIO_SENDDATAOUT	0x00000194
#define GPIO_CLRDATAOUT		0x00000190

#define IO_DIRECTION_OUT	0xFBFFFFFF	//(0<<26)
#define IO_OUTPUT_ON		0x04000000	//(1<<26)

int main(void)
{
	int fd;
	unsigned int counter = 0;

	void	*baseaddr;
	unsigned long *gpio_oe;
	unsigned long *gpio_dataout;
	unsigned long *gpio_senddataout;
	unsigned long *gpio_clrdataout;

	fd= open("/dev/mem", O_RDWR | O_SYNC);
	if(-1 != fd)
	{
		printf("Opening file /dev/mem -> succeeded!\n");

		baseaddr = mmap(0,
						GPIO_SIZE,
						PROT_READ|PROT_WRITE,
						MAP_SHARED,
						fd,
						GPIO0_BASE_START);

		if(baseaddr != NULL)
		{
			printf("mmap succeeded!\nAddress: %p\n", baseaddr);

			gpio_oe				= (unsigned long *) ((unsigned char *)baseaddr + GPIO_OE);
			gpio_dataout		= (unsigned long *) ((unsigned char *)baseaddr + GPIO_DATAOUT);
			gpio_senddataout	= (unsigned long *) ((unsigned char *)baseaddr + GPIO_SENDDATAOUT);
			gpio_clrdataout		= (unsigned long *) ((unsigned char *)baseaddr + GPIO_CLRDATAOUT);

			printf("GPIO_OE : %08x\n", gpio_oe);
			printf("GPIO_OE : %08x\n", gpio_dataout);
			printf("GPIO_SENDDATA : %08x\n", gpio_senddataout);
			printf("GPIO_CLRDATA : %08x\n", gpio_clrdataout);

			printf("LED Direction = OUT\n");
			*gpio_oe				&= IO_DIRECTION_OUT;
			printf("GPIO0_OE\n %#08x\n",*gpio_oe);
			sleep(1);
			
			while(counter < 5){
				*gpio_senddataout	|= IO_OUTPUT_ON;
				printf("LED ON : %08x\n", *gpio_dataout);
				sleep(1);

				*gpio_clrdataout	&= IO_OUTPUT_ON;
				printf("LED OFF : %08x\n", *gpio_dataout);
				sleep(1);

				counter++;
			}

			if( baseaddr != NULL)
			{
				printf("Unmap Memory\n");
				munmap(baseaddr, GPIO_SIZE);
				close(fd);
			}
		}
			else
			{
				printf("mmap failed!\n");
			}
	}
	else
	{
		printf("Opening file /dev/mem -> failed!\n");
	}

	return 0;
}

