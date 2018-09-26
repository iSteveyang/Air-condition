#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
//#include <sys/ioctl.h>
int MAX_LEN=255;
int main()
{
	int fd;
	int i;
	char c,sc;
	unsigned char buf1[32]={0xc0,0xc0,0xf9,0xf9,0xa4,0xa4,0xb0,0xb0,
                                0x99,0x99,0x92,0x92,0x82,0x82,0xf8,0xf8,
                                0x80,0x80,0x90,0x90,0x88,0x88,0x83,0x83,
                                0xc6,0xc6,0xa1,0xa1,0x86,0x86,0x8e,0x8e,
                                 };
	unsigned char buf[2]={0xff,0xff};

	fd=open("/dev/S3C2410_SPI0",O_RDWR);
	if(fd < 0){
		printf("####spi  device open fail####\n");
		return (-1);
	}
	
        write(fd,&buf,2);
       while(1)
	for(i=0;i<32;)
	{
	
	 write(fd,buf1+i,2);
            sleep(1);
         write(fd,buf,2);
         i=i+2;
        
        }
				
	close(fd);
	return 0;

}



