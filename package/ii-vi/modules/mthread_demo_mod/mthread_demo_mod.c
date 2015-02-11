/*
 * BeagleBone Black Evaluation Board Multi-Threading LED driver
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
#include <linux/kthread.h>

#include <linux/delay.h>	// mdelay(), ndelay(), msleep()
//#include <asm/delay.h> 		// udelay() - no larger than 2000us
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

#define DEVICE_NAME		"mthread_leds"
#define DRIVER_VER		"1.0.0"

#define MTH_NUM_LED		(2)

#define MTH_LED_USR0_GPIO_PIN	(53)
#define MTH_LED_USR1_GPIO_PIN	(54)

static int numParm = MTH_NUM_LED;
static int mth_periods[MTH_NUM_LED] = { 0, };

static const int led_pin_desc[MTH_NUM_LED] = { MTH_LED_USR0_GPIO_PIN, MTH_LED_USR1_GPIO_PIN };

static struct task_struct *kthread_t[MTH_NUM_LED];
static int led_init_map = 0;

static struct
{
	int gpio_pin;
	int led_on;				// 0: OFF; 1: ON
} mthread_led_ctrl[MTH_NUM_LED];

module_param_array(mth_periods, int, &numParm, S_IRUGO | S_IWUSR);

static void init_led_status(int id)
{
	gpio_set_value(mthread_led_ctrl[id].gpio_pin, 0);		// usr LED off
}

int led_flsh_thread(void *data)
{
	int id = *(int *)data;
//	printk(KERN_ALERT "thread id = %d\n", id);

	while(1)
	{
		set_current_state(TASK_INTERRUPTIBLE);
		if (kthread_should_stop()) break;
		gpio_set_value(mthread_led_ctrl[id].gpio_pin, mthread_led_ctrl[id].led_on = !mthread_led_ctrl[id].led_on);
//		printk(KERN_ALERT "thread%d=%d\n", id, mthread_led_ctrl[id].led_on);
		schedule_timeout(msecs_to_jiffies(mth_periods[id]));
	}
	printk("break\n");

	return 0;
}

struct file_operations mthread_led_fops = {
		.owner				= THIS_MODULE,
};

static struct miscdevice mthread_led_dev = {
		.minor		= MISC_DYNAMIC_MINOR,
		.name		= DEVICE_NAME,
		.fops		= &mthread_led_fops,
};

static int init_led_ctrl(void)
{
	int ret;
	int i;

	for (i = 0; i < MTH_NUM_LED; i++)
	{
		mthread_led_ctrl[i].gpio_pin = led_pin_desc[i];
		mthread_led_ctrl[i].led_on = 0;

		ret = gpio_request(mthread_led_ctrl[i].gpio_pin, 0);
		if (!ret)
		{
			led_init_map |= 1<<i;
			ret = gpio_direction_output(mthread_led_ctrl[i].gpio_pin, 0);
			if (!ret)
			{
				init_led_status(i);
			}
			else
			{
				printk(KERN_ALERT "GPIO pin %d set output direction failed!\n", led_pin_desc[i]);
			}
		}
		else
		{
			printk(KERN_ALERT "GPIO pin %d request failed!\n", led_pin_desc[i]);
		}
	}

	if (!ret)
	{
		ret = misc_register(&mthread_led_dev);
		if(!ret)
		{
//			printk(KERN_ALERT DEVICE_NAME " initialized\n");
		}
		else
		{
			printk(KERN_ALERT "Device register failed!\n");

		}
	}

	if (ret)
	{
		for(i = 0; i < MTH_NUM_LED; i++)
		{
			if (led_init_map & (1<<i))
			{
				gpio_free(mthread_led_ctrl[i].gpio_pin);
			}
		}
	}

	return ret;
}

static int mth_id[MTH_NUM_LED];

static int __init init_mthread_demo(void)
{
	int i;
	int ret;

	for (i = 0; i < MTH_NUM_LED; i++)
	{
		mth_id[i] = i;
		kthread_t[i] = NULL;
	}

//	printk(KERN_ALERT "MTHREAD_DEMO driver (" DRIVER_VER ") loaded to the kernel!\n");

	led_init_map = 0;

	ret = init_led_ctrl();

	if (!ret)
	{
		for (i = 0; i < MTH_NUM_LED; i++)
		{
			kthread_t[i] = kthread_run(led_flsh_thread, &mth_id[i], "MTH_LED_%d", i);
			if (IS_ERR(kthread_t[i]))
			{
				printk(KERN_ALERT "[thread/%d]: creating kthread failed!\n", i);
				ret = PTR_ERR(kthread_t[i]);
				kthread_t[i] = NULL;
				break;
			}
			else
			{
//				printk(KERN_ALERT "kernel thread %s created!\n", kthread_t[i]->comm);
			}
		}
	}

//	printk(KERN_ALERT "thread tick: %d:%d\n", mth_periods[0], mth_periods[1]);

	return ret;
}

static void __exit exit_mthread_demo(void)
{
	int i;
//	printk(KERN_ALERT "MTHREAD_DEMO BBB LED driver unloaded from the kernel!\n");

	for (i = 0; i < MTH_NUM_LED; i++)
	{
		gpio_set_value(mthread_led_ctrl[i].gpio_pin, 0);		// usr LED off
		gpio_free(mthread_led_ctrl[i].gpio_pin);

		kthread_stop(kthread_t[i]);
	}

//	printk(KERN_ALERT DEVICE_NAME " de-initialized\n");

	misc_deregister(&mthread_led_dev);
}

module_init(init_mthread_demo);
module_exit(exit_mthread_demo);

MODULE_AUTHOR("S.Yang");
MODULE_DESCRIPTION("A BeagleBone Black Multi-Thread Demo Module");
MODULE_VERSION(DRIVER_VER);
MODULE_LICENSE("GPL");
