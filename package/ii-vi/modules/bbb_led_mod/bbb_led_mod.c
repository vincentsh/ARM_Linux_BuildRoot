/*
 * BeagleBone Black Evalutation Board LED driver
 *
 * Date: 		2014.02.17
 * Author: 		Shuozhi Yang
 * Company:		II-VI Inc. Network Solution
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>
#include <asm/uaccess.h>

#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/delay.h>	// mdelay(), ndelay(), msleep()
//#include <asm/delay.h> 		// udelay() - no larger than 2000us
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <linux/ioctl.h>
#include <linux/fs.h>

#include "memdev.h"

#define DEVICE_NAME		"bbb_leds"
#define DRIVER_VER		"0.1.0"

#define BBB_LED_USR0_GPIO_PIN	(53)
#define BBB_LED_USR1_GPIO_PIN	(54)

#define LED_NUM_MODE			(2)
#define LED_MAN_MODE			(0)
#define LED_MAN_MODE_DESC		"manual"
#define LED_FLS_MODE			(1)
#define LED_FLS_MODE_DESC		"flashing"

#define LED_TIMER_INTERVAL		(50)		// interval 500ms

static int usrLed = 0;

const char *mode_desc[LED_NUM_MODE] = { LED_MAN_MODE_DESC, LED_FLS_MODE_DESC };

static struct
{
	int gpio_pin;
	int mode_id;			// 0: manuall; 1: flashing
	int led_on;				// 0: OFF; 1: ON
} led_ctrl;

module_param(usrLed, int, S_IRUGO | S_IWUSR);

static struct timer_list led_timer;
static void led_timer_isr(unsigned long tick);

static void init_led_status(void)
{
	gpio_set_value(led_ctrl.gpio_pin, 0);		// usr0 LED off
}

static void led_timer_isr(unsigned long data)
{
	if(led_ctrl.mode_id == LED_FLS_MODE)
	{
		gpio_set_value(led_ctrl.gpio_pin, led_ctrl.led_on = !led_ctrl.led_on);
	}
	led_timer.expires = jiffies + LED_TIMER_INTERVAL;
	add_timer(&led_timer);
}

static long bbb_led_ioctl(struct file *fd, unsigned int cmd, unsigned long arg)
{
	if (_IOC_TYPE(cmd) != MEMDEV_IOC_MAGIC)
	{
		return -EINVAL;
	}

	if (_IOC_NR(cmd) > MEMDEV_IOC_MAXNR)
	{
		return -EINVAL;
	}

	printk("here I am in kernel ioctl!\n");

	switch (cmd)
	{
		case MEMDEV_IOCOFF:
		{
			gpio_set_value(led_ctrl.gpio_pin, 0);
			return 0;
		}
		case MEMDEV_IOCON:
		{
			gpio_set_value(led_ctrl.gpio_pin, 1);
			return 0;
		}
		default:
		{
			return -EINVAL;
		}
	}
}

static int bbb_led_open(struct inode *inode, struct file *file)
{
//	printk("bbb_led opened\n");

	return 0;
}

static int bbb_led_close(struct inode *inode, struct file *file)
{
//	printk("bbb_led closed\n");

	return 0;
}

static int bbb_led_write(struct file *fd, const char __user *writeBuf, size_t writeSize, loff_t *i)
{
	char tmp[64];
//	printk(KERN_ALERT "bbb_led write call\n");

	copy_from_user(tmp, writeBuf, writeSize);
	tmp[writeSize-1] = 0;
//	printk(KERN_ALERT "string = %s, writeSize = %d\n", tmp, writeSize);

	if(!strcmp(mode_desc[LED_MAN_MODE], tmp))
	{
		printk(KERN_ALERT "manual mode selected\n");
		led_ctrl.mode_id = LED_MAN_MODE;
	}
	else if(!strcmp(mode_desc[LED_FLS_MODE], tmp))
	{
		printk(KERN_ALERT "flashing mode selected\n");
		led_ctrl.mode_id = LED_FLS_MODE;
	}
	else
	{
		printk(KERN_ALERT "invalid value!\n");
	}

	return writeSize;
}

static int bbb_led_read(struct file *fd, char __user *readBuf, size_t readSize, loff_t *i)
{
	static int read_complete = 0;
	char tmp[64];
	int ret = 0;
	int count;

//	printk(KERN_ALERT "bbb_led read call\n");

	if (read_complete == 1)
	{
		read_complete = 0;
		return 0;
	}

	sprintf(tmp, "%s%s", mode_desc[led_ctrl.mode_id], "\n");
	count = strlen(tmp);
	ret = copy_to_user(readBuf, tmp, count) ? -EFAULT : count;

	read_complete = 1;

	return ret;
}

struct file_operations led_fops = {
		.owner				= THIS_MODULE,
		.unlocked_ioctl		= bbb_led_ioctl,
		.open				= bbb_led_open,
		.release			= bbb_led_close,
		.write				= bbb_led_write,
		.read				= bbb_led_read,
};

static struct miscdevice led_dev = {
		.minor		= MISC_DYNAMIC_MINOR,
		.name		= DEVICE_NAME,
		.fops		= &led_fops,
};

void init_led_ctrl(void)
{
	led_ctrl.gpio_pin = (usrLed == 0) ? BBB_LED_USR0_GPIO_PIN : BBB_LED_USR1_GPIO_PIN;
	led_ctrl.led_on = 0;
	led_ctrl.mode_id = 0;	// default manual mode
}

static int __init init_bbb_led(void)
{
	int ret;

	init_led_ctrl();

	printk("BBB LED driver (" DRIVER_VER ") loaded to the kernel!\n");

	ret = gpio_request(led_ctrl.gpio_pin, 0);
	if (!ret)
	{
		ret = gpio_direction_output(led_ctrl.gpio_pin, 0);
		if (!ret)
		{
			ret = misc_register(&led_dev);
			if(!ret)
			{
//				printk(KERN_ALERT DEVICE_NAME " initialized\n");

				init_led_status();
				init_timer(&led_timer);

				led_timer.function = &led_timer_isr;
				led_timer.expires = jiffies + LED_TIMER_INTERVAL;
				led_timer.data = 0;

				add_timer(&led_timer);	// start timer
			}
		}
		else
		{
			printk(KERN_ALERT "GPIO pin set output direction failed!\n");
		}
	}
	else
	{
		printk(KERN_ALERT "GPIO pin request failed!\n");
	}

	return ret;
}

static void __exit exit_bbb_led(void)
{
	printk("BBB LED driver unloaded from the kernel!\n");

	gpio_set_value(led_ctrl.gpio_pin, 0);		// usr0 LED off
	gpio_free(led_ctrl.gpio_pin);
	del_timer_sync(&led_timer);

//	printk(KERN_ALERT DEVICE_NAME " de-initialized\n");

	misc_deregister(&led_dev);
}

module_init(init_bbb_led);
module_exit(exit_bbb_led);

MODULE_AUTHOR("S.Yang");
MODULE_DESCRIPTION("A BeagleBone Black LED Driver");
MODULE_VERSION(DRIVER_VER);
MODULE_LICENSE("GPL");
