#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
// #include <linux/uaccess.h>
MODULE_LICENSE("GPL");

#define MAJOR_NUM 235
#define DEVICE_NAME "my_dev"
uint8_t gpio_state = 0;
uint8_t rec_buf[10] = {0};

// File Operations
static ssize_t my_read(struct file *fp, char *buf, size_t count, loff_t *fpos)
{
    count = 1;
    if( copy_to_user(buf, rec_buf, count) > 0) {
        pr_err("ERROR: Not all the bytes have been copied to user\n");
    }
    pr_info("Read function : GPIO_21 = %d \n", gpio_state);
    return 0;
}
static ssize_t my_write(struct file *fp, const char *buf, size_t count, loff_t *fpos)
{
    if( copy_from_user( rec_buf, buf, count ) > 0) {
        pr_err("ERROR: Not all the bytes have been copied from user\n");
    }
    pr_info("Write Function : GPIO_21 Set = %c\n", rec_buf[0]);
    return count;
}
static int my_open(struct inode *inode, struct file *fp)
{
	printk("call open\n");
	return 0;
}
struct file_operations my_fops = {
	read: my_read,
	write: my_write,
	open: my_open
};

static int my_init(void)
{
	printk("call init\n");
	if (register_chrdev(MAJOR_NUM, DEVICE_NAME, &my_fops) < 0) {
		printk("Can not get major %d\n", MAJOR_NUM);
		return (-EBUSY);
	}
	printk("My device is started and the major is %d\n", MAJOR_NUM);
	return 0;
}
static void my_exit(void)
{
	unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
	printk("call exit\n");
}
module_init(my_init);
module_exit(my_exit);