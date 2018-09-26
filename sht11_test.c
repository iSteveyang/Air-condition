#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/ioctl.h>

#define TEMP 0
#define HUMI 1

void calc_sht11(float *p_humidity,float *p_temprature)
{
   const float C1=-0.40;
   const float C2=0.0405;
   const float C3=-0.0000028;
   const float T1=0.01;
   const float T2=0.00008;

   float rh=*p_humidity;
   float t=*p_temprature;
   float rh_lin;
   float rh_true;
   float t_C;

   t_C=t*0.01-40;
   rh_lin=C3*rh*rh+C2*rh+C1;
   rh_true=(t_C-25)*(T1+T2*rh)+rh_lin;
   if(rh_true>100)rh_true=100;
   if(rh_true<0.1)rh_true=0.1;

   *p_temprature=t_C;
   *p_humidity=rh_true;
}

float calc_dewpoint(float h,float t)
{
   float k,dew_point;
   k=(log10(h)-2)/0.4343+(17.62*t)/(243.12+t);
   dew_point=243.12*k/(17.62-k);
   return dew_point;
}

void delay(int time)
{
   int i;
   for(i=0;i<time*1000;)
     i++;
}

int main(void)
{
    int fd,ret,i;
    	unsigned int value_t=0;
        unsigned int value_h=0;
        float fvalue_t,fvalue_h;
        float dew_point;
	fd = open("/dev/sht11",0);

	if(fd<0)
	{
		printf("open /dev/sht11 error!\n");
		return -1;
	}
 
	for(;;)
	{
                fvalue_t=0.0,fvalue_h=0.0;value_t=0;value_h=0;
		ioctl(fd,0);
		ret=read(fd,&value_t,sizeof(value_t));
                if(ret<0)
		{
			printf("read err!\n");
			continue;
		}
                sleep(1);
		
                value_t=value_t&0x3fff;
                //printf("value_t=%d\n",value_t);
                fvalue_t=(float)value_t;

                ioctl(fd,1);
		ret=read(fd,&value_h,sizeof(value_h));
               // printf("value_h=%d\n",value_h);
                sleep(1);
		if(ret<0)
		{
			printf("read err!\n");
			continue;
		}
                value_h=value_h&0xfff;
                fvalue_h=(float)value_h;
                calc_sht11(&fvalue_h,&fvalue_t);
                dew_point=calc_dewpoint(fvalue_h,fvalue_t);
                printf("temp:%fc humi:%f%% dew point:%fc\n",fvalue_t,fvalue_h,dew_point);
                sleep(1);
               
}
}
