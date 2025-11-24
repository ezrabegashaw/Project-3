#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#define DIR_NAME  "mydir"
#define FILE_NAME "myinfo"

static struct proc_dir_entry *dir_entry, *info_entry;
static unsigned char *buffer;                     // one page we map to user
static unsigned char init_array[12] = {0,1,2,3,4,5,6,7,8,9,10,11};

static int my_map(struct file *filp, struct vm_area_struct *vma)
{
    unsigned long pfn = virt_to_phys(buffer) >> PAGE_SHIFT;
    size_t size = vma->vm_end - vma->vm_start;
    if (size > PAGE_SIZE) return -EINVAL;

    vma->vm_flags |= VM_DONTEXPAND | VM_DONTDUMP;
    if (remap_pfn_range(vma, vma->vm_start, pfn, size, vma->vm_page_prot))
        return -EAGAIN;
    return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops myproc_fops = {
    .proc_mmap = my_map,
};
#else
static const struct file_operations myproc_fops = {
    .mmap = my_map,
};
#endif

static void allocate_memory(void)
{
    buffer = (unsigned char *)kmalloc(PAGE_SIZE, GFP_KERNEL);
    if (buffer) {
        memcpy(buffer, init_array, sizeof(init_array));
        SetPageReserved(virt_to_page(buffer));   // for older kernels
    }
}

static void clear_memory(void)
{
    if (buffer) {
        ClearPageReserved(virt_to_page(buffer));
        kfree(buffer);
        buffer = NULL;
    }
}

static int __init myproc_init(void)
{
    allocate_memory();
    if (!buffer) return -ENOMEM;

    dir_entry = proc_mkdir(DIR_NAME, NULL);
    if (!dir_entry) { clear_memory(); return -ENOMEM; }

    info_entry = proc_create(FILE_NAME, 0, dir_entry, &myproc_fops);
    if (!info_entry) {
        remove_proc_entry(DIR_NAME, NULL);
        clear_memory();
        return -ENOMEM;
    }
    printk(KERN_INFO "init myproc module successfully\n");
    return 0;
}

static void __exit myproc_exit(void)
{
    remove_proc_entry(FILE_NAME, dir_entry);
    remove_proc_entry(DIR_NAME, NULL);
    clear_memory();
    printk(KERN_INFO "remove myproc module successfully\n");
}

module_init(myproc_init);
module_exit(myproc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Project 3 - Part 3: mmap from /proc to user space");
