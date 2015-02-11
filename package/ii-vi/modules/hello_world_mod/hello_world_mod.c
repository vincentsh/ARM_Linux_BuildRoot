
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>

static int __init init_hello_world(void)
{
	printk(KERN_ALERT "KERN_ALERT: Hello world from Yang!\n");
	printk(KERN_CRIT "KERN_CRIT: Hello world from Yang!\n");
	printk(KERN_WARNING "KERN_WARNING: Hello world from Yang!\n");
	printk(KERN_NOTICE "KERN_NOTICE: Hello world from Yang!\n");
	printk(KERN_INFO "KERN_INFO: Hello world from Yang!\n");
	printk(KERN_DEBUG "KERN_DEBUG: Hello world from Yang!\n");
	printk("KERN_DEFAULT: Hello world from Yang!\n");

	return 0;
}

static void __exit exit_hello_world(void)
{
	printk(KERN_ALERT "good bye world from Yang!\n");
}

module_init(init_hello_world);
module_exit(exit_hello_world);

MODULE_AUTHOR("S.Yang");
MODULE_DESCRIPTION("A Hello World Module");
MODULE_VERSION("0.1.0");
MODULE_LICENSE("GPL");
