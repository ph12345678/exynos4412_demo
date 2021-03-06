//生成设备节点信息
/**
 * 包含初始化宏定义的头文件，代码中的module_init与module_exit在此文件中
 */
#include <linux/init.h>
/**
 * 包含初始化加载模块的头文件，代码中的MODULE_LICENSE在此头文件中
 */
#include <linux/module.h>
/**
 * 驱动注册头文件，包括驱动注册结构体、注册与卸载函数
 */
#include <linux/platform_device.h>
/**
 * 杂项设备结构体描述头文件，包括设备的注册、设备的卸载
 */
#include <linux/miscdevice.h>
/**
 * 注册设备文件节点的结构体头文件
 */
#include <linux/fs.h>

/**
 * Linux中申请gpio的头文件
 */
#include <linux/gpio.h>
/*三星平台的GPIO配置函数头文件*/
/*三星平台EXYNOS系列平台，GPIO配置参数宏定义头文件*/
#include <plat/gpio-cfg.h>
#include <mach/gpio.h>
/*三星平台4412平台，GPIO宏定义头文件*/
#include <mach/gpio-exynos4.h>

//驱动注册名称，此名称一定要与注册的设备名称对应
#define DRIVER_NAME "hello_ctl" 
//生成的设备节点名称，此名称显示在/dev目录下，应用程序使用此文件名称来实现应用
#define DEVICE_NAME "hello_led"

MODULE_LICENSE("Dual BSD/GPL"); //声明是开源的，没有内核版本限制
MODULE_AUTHOR("KQZ"); //声明作者

/**********start generate devicenode*********/
static int hello_open  (struct inode * node, struct file * f)
{
	printk(KERN_EMERG"\t generate the device node in hello open function----------\n");
	return 0;
}
static int hello_release (struct inode * node , struct file * f)
{
	printk(KERN_EMERG"\t generate the device node in hello relese function ------\n");
	return 0;
}
static long hello_ioctl(struct file * f, unsigned int cmd , unsigned long arg)
{
	printk(KERN_EMERG"\t generate device node the cmd is %d ,arg is %d",cmd,arg);

	if(cmd > 1){
		printk(KERN_EMERG "cmd is 0 or 1\n");
		return -1;
	}
	if(arg > 1){
		printk(KERN_EMERG "arg is only 1\n");
		return -1;
	}
	
	gpio_set_value(EXYNOS4_GPL2(0),cmd);
	return 0;
}
static struct file_operations hello_ops={
	.owner=THIS_MODULE,
	.open=hello_open,
	.release=hello_release,
	.unlocked_ioctl=hello_ioctl,
};

static struct miscdevice hello_dev={
	.minor = MISC_DYNAMIC_MINOR, //自动生成设备节点次设备号
	.name = DEVICE_NAME,
	.fops = &hello_ops,
};


/**********end generate devicenode*********/
/**
 * 在hello_probe函数中，进行gpio的申请，其中申请操作，用的是linux内核提供函数
 * 进行参数设置的时候，用到的是三星平台提供的宏定义
 * @param  pdv [description]
 * @return     [description]
 */
static int hello_probe(struct platform_device * pdv)
{
	int gpio_request_state;
	int miscdevice_state;
	printk(KERN_EMERG "\tthe hello driver probe init -----\n");

	//进行gpio的申请操作
	gpio_request_state = gpio_request(EXYNOS4_GPL2(0),"LEDS");
	if(gpio_request_state <0){
		printk(KERN_EMERG"request the EXYNOS4_GPL2 is failed \n");
		return gpio_request_state;
	}
	//对EXYNOS4_GPL2进行配置，配置为输出管脚
	//此处用到的是三星平台函数
	s3c_gpio_cfgpin(EXYNOS4_GPL2(0),S3C_GPIO_OUTPUT);
	//对管脚进行置0操作，
	//gpio_set_value为linux内核提供函数
	gpio_set_value(EXYNOS4_GPL2(0),0);

	//进行杂项设备的注册
	miscdevice_state=misc_register(&hello_dev);
	printk(KERN_EMERG"\t the register miscdevice state is %d \n",miscdevice_state);
	return 0;
}

static int hello_remove(struct platform_device * pdv)
{
	int miscdevice_state;
	//进行杂项设备的卸载
	miscdevice_state=misc_deregister(&hello_dev);
	printk(KERN_EMERG"\t the deregister miscdevice state is %d \n",miscdevice_state);
	return 0;
}

static void hello_shutdown(struct platform_device * pdv)
{
	//return 0;
}


static int hello_suspend(struct platform_device * pdv,pm_message_t state)
{
	return 0;
}


static int hello_resume(struct platform_device * pdv)
{
	return 0;
}

/**
 * 注册驱动结构体
 */
struct platform_driver hello_driver={
	.probe=hello_probe,
	.remove=hello_remove,
	.shutdown=hello_shutdown,
	.suspend=hello_suspend,
	.resume=hello_resume,
	.driver={
		.name=DRIVER_NAME,
		.owner=THIS_MODULE,
	}
};

/**
 * [驱动加载程序入口函数]
 * @return  [description]
 */
static int hello_init(void)
{
	int driver_state;

    /*
     打印信息，KERN_EMERG 表示紧急信息
     */
    printk(KERN_EMERG "Hello World enter !\n");
    //注册设备驱动
    driver_state=platform_driver_register(&hello_driver);
    printk(KERN_EMERG "the driver register state is %d \n",driver_state);
    return 0;
}

/**
 * [驱动模块卸载程序执行的函数入口]
 */
static void hello_exit(void)
{
	//卸载设备驱动
	platform_driver_unregister(&hello_driver);	
    printk(KERN_EMERG "Hello World exit !\n");
}

module_init(hello_init); //模块入口
module_exit(hello_exit); //模块出口