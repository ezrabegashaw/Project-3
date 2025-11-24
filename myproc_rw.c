#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/version.h>

#define PROC_NAME "myproc"
#define MAX_LEN   4096

static struct proc_dir_entry *proc_entry;
static char *info_buf;
static size_t info_len;

static ssize_t myproc_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos)
{
    if (*ppos >= info_len) return 0;                         // EOF
    if (count > info_len - *ppos) count = info_len - *ppos;
    if (copy_to_user(ubuf, info_buf + *ppos, count)) return -EFAULT;
    *ppos += count;
    return count;
}

static ssize_t myproc_write(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos)
{
    if (count > MAX_LEN - 1) count = MAX_LEN - 1;
    if (copy_from_user(info_buf, ubuf, count)) return -EFAULT;
    info_len = count;
    info_buf[info_len] = '\0';
    return count;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops myproc_ops = {
    .proc_read  = myproc_read,
    .proc_write = myproc_write,
};
#else
static const struct file_operations myproc_ops = {
    .read  = myproc_read,
    .write = myproc_write,
};
#endif

static int __init myproc_init(void)
{
    info_buf = kmalloc(MAX_LEN, GFP_KERNEL);
    if (!info_buf) return -ENOMEM;
    info_len = 0;

    proc_entry = proc_create(PROC_NAME, 0666, NULL, &myproc_ops);
    if (!proc_entry) {
        kfree(info_buf);
        return -ENOMEM;
    }
    printk(KERN_INFO "test_proc created.\n");
    return 0;
}

static void __exit myproc_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    kfree(info_buf);
}

module_init(myproc_init);
module_exit(myproc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Project 3 - Part 2: /proc read/write");
