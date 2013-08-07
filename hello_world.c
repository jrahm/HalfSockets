#include <linux/module.h>
#include <linux/kernel.h>

int init_module( void ) {
	printk("<1>Hello, World! 1.\n");

	return 0;
}

void cleanup_module( void ) {
	printk(KERN_ALERT "Goodbye World! 1.\n");
}

MODULE_LICENSE("GPL");
