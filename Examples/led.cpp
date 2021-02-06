#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

FILE *fp;

int main (int argc, char** argv)
{
	char set_value[4];	//create a variable to store
	int toggle = 0;		//Integer to keep track
				//Using sysfs we need to write"44" to /sys/class/gpio/export
				//This create the folder/sys/class/gpio/gpio44
	if((fp = fopen("/sys/class/gpio/export","ab")) == NULL)
	{
		printf("Can't open export file.\n");
		exit(1);
	}
	rewind(fp);		 //set pointer to begining of the file
	strcpy(set_value, "44"); //GPIO1_12
	fwrite(&set_value, sizeof(char),2,fp);
	fclose(fp);

	printf("...export file accessed, new pin now accessible\n");

	//Set direction
	//Open the LED's sysfs file in binary for reading and writing, stor file pointer in fp
	if((fp = fopen("/sys/class/gpio/gpio44/direction","rb+")) == NULL)
	{
		printf("Can't open direction file.\n");
		exit(1);
	}
	//Set pointer to begining of the file
	rewind(fp);
	//Write our value of "out" to the file
	strcpy(set_value,"out");
	fwrite(&set_value,sizeof(char),3,fp);
	fclose(fp);

	printf("...direction set to output\n");

	//Set value
	//Open the LED's sysfs file in binary for reading and writing, stor file pointer in fp
	if((fp=fopen("/sys/class/gpio/gpio44/direction","rb+")) == NULL)
	{
		printf("Can't open value file.\n");
		exit(1);
	}
	//Set pointer to begining of the file
	rewind(fp);
	//Write our value of "1" to the file
	strcpy(set_value,"1");
	fwrite(&set_value,sizeof(char),1,fp);
	fclose(fp);

	printf("...value set to 1.\n");

	//Run as infinite loop - will require Ctrl-C to exit this program
	while(1)
	{
		//Set it so we know the starting value in case something above doesn't leave it as 1
		strcpy(set_value,"1");

		if((fp=fopen("/sys/class/gpio/gpio44/value","rb+")) == NULL)
		{
			printf("Can't open value file.\n");
			exit(1);
		}

		toggle = !toggle;

		if(toggle)
		{
			//Set pointer to begining of the file
			rewind(fp);
			//Write our value of "1" to the file
			strcpy(set_value,"1");
			fwrite(set_value,sizeof(char),1,fp);
			fclose(fp);

			printf("...value set to %s.\n",set_value);
		}
		else
		{
			//Set pointer to begining of the file
			rewind(fp);
			//Write our value of "0" to the file
			strcpy(set_value,"0");
			fwrite(set_value,sizeof(char),1,fp);
			fclose(fp);

			printf("...value set to %s.\n",set_value);
		}
		//Pause for one second
		sleep(1);
	}
	return 0;
}

