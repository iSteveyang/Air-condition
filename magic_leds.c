#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <asm/io.h>
#include <asm/unistd.h>
#include <asm/plat-s3c24xx/regs-spi.h>
#include <asm-arm/arch-s3c2410/regs-gpio.h>

#define SPI_MAJORE 25

#define DEVICE_NAME  "S3C2410_SPI0"
struct class *leds_class;
struct cdev dev;
#define rSPCON0  ((volatile unsigned long *)r_SPCON0)   
#define rSPSTA0  ((volatile unsigned long *)r_SPSTA0)   
#define rSPPIN0  ((volatile unsigned long *)r_SPPIN0)   
#define rSPPRE0  ((volatile unsigned long *)r_SPPRE0)   
#define rSPTDAT0 ((volatile unsigned long *)r_SPTDAT0)  
#define rSPRDAT0 ((volatile unsigned long *)r_SPRDAT0)  

#define rCLKCON ((volatile unsigned long *)r_CLKCON)

unsigned long *r_SPCON0,*r_SPSTA0,*r_SPPIN0,*r_SPPRE0,*r_SPTDAT0,*r_SPRDAT0,*r_CLKCON;

int address_map(void)
{
    r_CLKCON = ioremap(0x4C00000c,4);
    r_SPCON0 = ioremap(0x59000000,4);
    r_SPSTA0 = ioremap(0x59000004,4);
    r_SPPIN0 = ioremap(0x59000008,4);
    r_SPPRE0 =  ioremap(0x5900000c,4);
    r_SPTDAT0 = ioremap(0x59000010,4);
    r_SPRDAT0 = ioremap(0x59000014,4);
    return 0;
}

static ssize_t spi_open(struct inode *inode, struct file *file)
{
    unsigned long temp; 

    temp = __raw_readl(S3C2410_GPBUP); 
    temp &= (~((1<<3)|(1<<10)|(1<<9))); 
    __raw_writel(temp,S3C2410_GPBUP);  
  
    temp=__raw_readl(S3C2410_GPBCON);  
    temp &= (~((3<<(3*2))|(3<<(10*2))|(3<<(9*2))));
    temp |=((0<<(3*2))|(0<<(10*2))|(1<<(9*2)));
    __raw_writel(temp,S3C2410_GPBCON);
   
    temp = __raw_readl(S3C2410_GPEUP);  
    temp &=(~((1<<11)|(1<<12)|(1<<13)));
    __raw_writel(temp,S3C2410_GPEUP);

    temp  = __raw_readl(S3C2410_GPECON);  
    temp &= (~((3<<22)|(3<<24)|(3<<26)));
    temp |= ((2<<22)|(2<<24)|(2<<26));
    __raw_writel(temp,S3C2410_GPECON);
  
	s3c2410_gpio_cfgpin(S3C2410_GPH0,S3C2410_GPH0_OUTP);  
	s3c2410_gpio_pullup(S3C2410_GPH0, 0);

    temp = __raw_readl(S3C2410_GPGUP); 
    temp &= (~((1<<2)|(1<<3))); 
    __raw_writel(temp,S3C2410_GPGUP);  
   
    temp=__raw_readl(S3C2410_GPGCON);  
    temp &= (~((3<<(2*2))|(3<<(3*2))));
    temp |=((1<<(2*2))|(1<<(3*2)));
    __raw_writel(temp,S3C2410_GPGCON);

    temp = __raw_readl(rCLKCON);
    temp |= 0x40000; 
    __raw_writel(temp,rCLKCON);
 
    __raw_writel(0x0d,rSPPRE0);

    __raw_writel(0x19,rSPCON0);

    __raw_writel(0,rSPPIN0);
 
    return 0;
}

static ssize_t spi_release(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t spi_read(struct file *file, char __user *buffer, long count, loff_t *ppos)
{
    unsigned long temp;
    unsigned char c;
    int ret;
    if(count != 1) return -EFAULT;

    c=0x00;
    temp = __raw_readl(S3C2410_GPBDAT); 
    if(temp&(1<<3)) c |= 0x01;
    if(temp&(1<<10)) c |= 0x02;         
    ret=copy_to_user(buffer,&c,1);
    
    return 1;
}

static ssize_t spi_write(struct file *file, const char __user *buffer, long count, loff_t *ppos)
{
    int i;
    unsigned char ptr[count];


    if (copy_from_user(ptr, buffer, count))return -EFAULT;
 
    s3c2410_gpio_setpin(S3C2410_GPH0,0); 
    for(i=0;i<count;i++)
    { 
	while(((__raw_readl(rSPSTA0)) & 0x01) == 0x00);
        {
            __raw_writel(ptr[i],rSPTDAT0);
            udelay(10);
        } 
    }
    s3c2410_gpio_setpin(S3C2410_GPH0,1); 

    return count;
}


static ssize_t spi_ioctl(
    struct inode *inode, 
    struct file *file, 
    unsigned int cmd, 
    unsigned long arg)
{
    unsigned long temp;

    if (arg > 4) {
        return -EINVAL;
    }
    
    switch(cmd) {

    case 0:
        __raw_writel(arg,rSPCON0);
        return 0;

    case 1:
        __raw_writel(arg,rSPPRE0);
        return 0;

    case 2:
        switch(arg)
        {
        case 0:
                temp = __raw_readl(S3C2410_GPGDAT); 
                temp &= (~(1<<2));      
                __raw_writel(temp,S3C2410_GPGDAT); 
                break;
        case 1:
                temp = __raw_readl(S3C2410_GPGDAT); 
                temp &= (~(1<<3));      
                __raw_writel(temp,S3C2410_GPGDAT); 
                break;
        case 2:
                temp = __raw_readl(S3C2410_GPBDAT); 
                temp &= (~(1<<9));      
                __raw_writel(temp,S3C2410_GPBDAT); 
                break;
        default:
                return -EINVAL;
        }
        return 0;

    case 3:
        switch(arg)
        {
        case 0:
                temp = __raw_readl(S3C2410_GPGDAT); 
                temp |= (1<<2);     
                __raw_writel(temp,S3C2410_GPGDAT); 
                break;
        case 1:
                temp = __raw_readl(S3C2410_GPGDAT); 
                temp |= (1<<3);      
                __raw_writel(temp,S3C2410_GPGDAT); 
                break;
        case 2:
                temp = __raw_readl(S3C2410_GPBDAT); 
                temp |= (1<<9);      
                __raw_writel(temp,S3C2410_GPBDAT); 
                break;
        default:
                return -EINVAL;
        }
        return 0;

    default:
        return -EINVAL;
    }
}
static struct file_operations s3c2410_spi0_fops=
{
	.owner   = THIS_MODULE,
	.open    = spi_open,
	.write   = spi_write,
	.read    = spi_read,
         .ioctl   = spi_ioctl, 
	.release = spi_release,	
};

static void spi_setup_cdev(void)
{
	int err,devno = MKDEV(SPI_MAJORE,0);
	cdev_init(&dev,&s3c2410_spi0_fops);
	dev.owner = THIS_MODULE;
	dev.ops = &s3c2410_spi0_fops;
	err = cdev_add(&dev,devno,1);
	if(err)
		printk(KERN_NOTICE"Error cdev_add!!!");
}

static int __init s3c2410_spi0_init(void)
{
	int ret;

	dev_t devno = MKDEV(SPI_MAJORE,0);
	address_map();
	ret = register_chrdev_region(devno, 1, DEVICE_NAME);
	printk(DEVICE_NAME "initializing\n");
	if (ret<0)
	{
		printk(DEVICE_NAME "cann't register major numer\n");
		return ret;
	}
	printk(DEVICE_NAME "initialized\n");
	spi_setup_cdev();
	leds_class=class_create(THIS_MODULE,"led_class");
        if(IS_ERR(leds_class))
        {
		printk("err:failed in creating class.\n");
                return -1;
        }
        class_device_create(leds_class,NULL,MKDEV(SPI_MAJORE,0),NULL,"S3C2410_SPI%d",0);
	return 0;

}

static void __exit s3c2410_spi0_exit(void)
{
	class_device_destroy(leds_class,MKDEV(SPI_MAJORE,0));
        class_destroy(leds_class);
	cdev_del(&dev);
	unregister_chrdev_region(MKDEV(SPI_MAJORE,0), 1);
}

module_init(s3c2410_spi0_init);
module_exit(s3c2410_spi0_exit);

MODULE_AUTHOR("xw_tech@126.com"); 
MODULE_DESCRIPTION("led driver for UP_Magic");
MODULE_LICENSE("GPL");

