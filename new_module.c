#include <linux/module.h>
#include <linux/kernel.h>

static int __init init_new_module(void)
{
    printk(KERN_INFO "Hello, world!\n");
    return 0;
}

static void __exit exit_new_module(void)
{
    printk(KERN_INFO "Goodbye, world!\n");
}

module_init(init_new_module);
module_exit(exit_new_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Project 3 - Part 1: Hello world kernel module");
