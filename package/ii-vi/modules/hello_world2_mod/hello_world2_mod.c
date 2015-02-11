
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>

#define N 5
static int intParm = 0;
static char* stringParm = "Oh my Yang!";
static int arrayParm[N] = {-1, -1, -1, -1, -1};
static int numParm = N;

module_param(intParm, int, S_IRUGO);
module_param(stringParm, charp, S_IRUGO);
module_param_array(arrayParm, int, &numParm, S_IRUGO);

static int __init init_hello_world(void)
{
	int i;
	printk(KERN_ALERT "hello world from Yang!\n"
			"My Int Parm (intParm) = %d\n"
			"My String Parm (stringParm) = %s\n"
			"My IntArray Parm (arrayParm) = ", intParm, stringParm);

	for(i = 0; i < N; i++)
		printk(KERN_ALERT "%d ", arrayParm[i]);
	printk(KERN_ALERT "\n");

	printk(KERN_ALERT "Current Process PID = %d\n"
			"Current Process Name = %s\n", current->pid, current->comm);

	return 0;
}

static void __exit exit_hello_world(void)
{
	printk(KERN_ALERT "good bye world from Yang!\n");
}

module_init(init_hello_world);
module_exit(exit_hello_world);

MODULE_AUTHOR("S.Yang");
MODULE_DESCRIPTION("A Hello World Module2");
MODULE_VERSION("0.1.0");
MODULE_LICENSE("GPL");
