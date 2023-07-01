/*
**************************************************************************
**
* \file
led_driver.c
* \details
Simple GPIO driver explanation
* \author
EmbeTronicX
* \Tested with Linux raspberrypi 5.4.51-v7l+
******************************************************************************
*/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/uaccess.h> //copy_to/from_user()
#include <linux/gpio.h>
//GPIO
//LED is connected to this GPIO
#define GPIO_21 (21)
#define GPIO_20 (20)
#define GPIO_16 (16)
#define GPIO_12 (12)
#define GPIO_18 (18) 
#define GPIO_15 (15)
#define GPIO_14 (14)
#define GPIO_4 (4)
#define GPIO_27 (27)
#define GPIO_22 (22)

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);
/*************** Driver functions **********************/
static int
etx_open(struct inode *inode, struct file *file);
static int
etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp,
char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp,
const char *buf, size_t len, loff_t * off);
/******************************************************/
//File operation structure
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = etx_read,
    .write = etx_write,
    .open = etx_open,
    .release = etx_release,
};
/*
** This function will be called when we open the Device file
*/
static int etx_open(struct inode *inode, struct file *file)
{
    pr_info("Device File Opened...!!!\n");
    return 0;
}
/*
** This function will be called when we close the Device file
*/
static int etx_release(struct inode *inode, struct file *file)
{
    pr_info("Device File Closed...!!!\n");
    return 0;
}
/*
** This function will be called when we read the Device file
*/
static ssize_t etx_read(struct file *filp,
char __user *buf, size_t len, loff_t *off)
{
    uint8_t gpio_state0 = 0;
    uint8_t gpio_state1 = 0;
    uint8_t gpio_state2 = 0;
    uint8_t gpio_state3 = 0;
    uint8_t gpio_state4 = 0;
    uint8_t gpio_state5 = 0;
    uint8_t gpio_state6 = 0;
    uint8_t gpio_state7 = 0;
    uint8_t gpio_state8 = 0;
    uint8_t gpio_state9 = 0;
//reading GPIO value
    gpio_state0 = gpio_get_value(GPIO_21); //7-g
    gpio_state1 = gpio_get_value(GPIO_20); //7-f
    gpio_state2 = gpio_get_value(GPIO_16); //7-a
    gpio_state3 = gpio_get_value(GPIO_12); //7-b
    gpio_state4 = gpio_get_value(GPIO_18); //7-e
    gpio_state5 = gpio_get_value(GPIO_15); //7-d
    gpio_state6 = gpio_get_value(GPIO_14); //7-c
    gpio_state7 = gpio_get_value(GPIO_4); //Led-a
    gpio_state8 = gpio_get_value(GPIO_27); //Led-b
    gpio_state9 = gpio_get_value(GPIO_22); //Led-c

//write to user
    len = 1;
    if( copy_to_user(buf, &gpio_state0, len) > 0) {
        pr_err("ERROR: Not all the bytes have been copied to user\n");
    }
    pr_info("Read function : GPIO_21 = %d \n", gpio_state0);
    pr_info("Read function : GPIO_20 = %d \n", gpio_state1);
    pr_info("Read function : GPIO_16 = %d \n", gpio_state2);
    pr_info("Read function : GPIO_12 = %d \n", gpio_state3);
    pr_info("Read function : GPIO_18 = %d \n", gpio_state4);
    pr_info("Read function : GPIO_15 = %d \n", gpio_state5);
    pr_info("Read function : GPIO_14 = %d \n", gpio_state6);
    pr_info("Read function : GPIO_4 = %d \n", gpio_state7);
    pr_info("Read function : GPIO_27 = %d \n", gpio_state8);
    pr_info("Read function : GPIO_22 = %d \n", gpio_state9);
    
    return 0;
}
/*
** This function will be called when we write the Device file
*/
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
    uint8_t rec_buf[10] = {0};
    if( copy_from_user( rec_buf, buf, len ) > 0) {
        pr_err("ERROR: Not all the bytes have been copied from user\n");
    }
    pr_info("Write Function : GPIO_21 Set = %c\n", rec_buf[0]);
    if (rec_buf[0]=='0') {
        gpio_set_value(GPIO_21, 1);
        gpio_set_value(GPIO_20, 0);
        gpio_set_value(GPIO_16, 0);
        gpio_set_value(GPIO_12, 0);
        gpio_set_value(GPIO_18, 0);
        gpio_set_value(GPIO_15, 0);
        gpio_set_value(GPIO_14, 0);

    }
    else if (rec_buf[0]=='1') {
        gpio_set_value(GPIO_21, 1);
        gpio_set_value(GPIO_20, 1);
        gpio_set_value(GPIO_16, 1);
        gpio_set_value(GPIO_12, 0);
        gpio_set_value(GPIO_18, 1);
        gpio_set_value(GPIO_15, 1);
        gpio_set_value(GPIO_14, 0);
    }
    else if (rec_buf[0]=='2') {
        gpio_set_value(GPIO_21, 0);
        gpio_set_value(GPIO_20, 1);
        gpio_set_value(GPIO_16, 0);
        gpio_set_value(GPIO_12, 0);
        gpio_set_value(GPIO_18, 0);
        gpio_set_value(GPIO_15, 0);
        gpio_set_value(GPIO_14, 1);

    }
    else if (rec_buf[0]=='3') {
        gpio_set_value(GPIO_21, 0);
        gpio_set_value(GPIO_20, 1);
        gpio_set_value(GPIO_16, 0);
        gpio_set_value(GPIO_12, 0);
        gpio_set_value(GPIO_18, 1);
        gpio_set_value(GPIO_15, 0);
        gpio_set_value(GPIO_14, 0);
    }
    else if (rec_buf[0]=='4') {
        gpio_set_value(GPIO_21, 0);
        gpio_set_value(GPIO_20, 0);
        gpio_set_value(GPIO_16, 1);
        gpio_set_value(GPIO_12, 0);
        gpio_set_value(GPIO_18, 1);
        gpio_set_value(GPIO_15, 1);
        gpio_set_value(GPIO_14, 0);
    }
    else if (rec_buf[0]=='5') {
        gpio_set_value(GPIO_21, 0);
        gpio_set_value(GPIO_20, 0);
        gpio_set_value(GPIO_16, 0);
        gpio_set_value(GPIO_12, 1);
        gpio_set_value(GPIO_18, 1);
        gpio_set_value(GPIO_15, 0);
        gpio_set_value(GPIO_14, 0);
    }
    else if (rec_buf[0]=='6') {
        gpio_set_value(GPIO_21, 0);
        gpio_set_value(GPIO_20, 0);
        gpio_set_value(GPIO_16, 0);
        gpio_set_value(GPIO_12, 1);
        gpio_set_value(GPIO_18, 0);
        gpio_set_value(GPIO_15, 0);
        gpio_set_value(GPIO_14, 0);
    }
    else if (rec_buf[0]=='7') {
        gpio_set_value(GPIO_21, 1);
        gpio_set_value(GPIO_20, 1);
        gpio_set_value(GPIO_16, 0);
        gpio_set_value(GPIO_12, 0);
        gpio_set_value(GPIO_18, 1);
        gpio_set_value(GPIO_15, 1);
        gpio_set_value(GPIO_14, 0);
    }
    else if (rec_buf[0]=='8') {
        gpio_set_value(GPIO_21, 0);
        gpio_set_value(GPIO_20, 0);
        gpio_set_value(GPIO_16, 0);
        gpio_set_value(GPIO_12, 0);
        gpio_set_value(GPIO_18, 0);
        gpio_set_value(GPIO_15, 0);
        gpio_set_value(GPIO_14, 0);
    }
    else if (rec_buf[0]=='9') {
        gpio_set_value(GPIO_21, 0);
        gpio_set_value(GPIO_20, 0);
        gpio_set_value(GPIO_16, 0);
        gpio_set_value(GPIO_12, 0);
        gpio_set_value(GPIO_18, 1);
        gpio_set_value(GPIO_15, 1);
        gpio_set_value(GPIO_14, 0);
    }
    else if (rec_buf[0]=='a') {
        gpio_set_value(GPIO_4, 1);
    }
    else if (rec_buf[0]=='b') {
        gpio_set_value(GPIO_27, 1);
    }
    else if (rec_buf[0]=='c') {
        gpio_set_value(GPIO_22, 1);
    }
    else if (rec_buf[0]=='d') {
        gpio_set_value(GPIO_4, 0);
    }
    else if (rec_buf[0]=='e') {
        gpio_set_value(GPIO_27, 0);
    }
    else if (rec_buf[0]=='f') {
        gpio_set_value(GPIO_22, 0);
    }
    else {
    	gpio_set_value(GPIO_21, 1);
        gpio_set_value(GPIO_20, 1);
        gpio_set_value(GPIO_16, 1);
        gpio_set_value(GPIO_12, 1);
        gpio_set_value(GPIO_18, 1);
        gpio_set_value(GPIO_15, 1);
        gpio_set_value(GPIO_14, 1);
        pr_err("Unknown command : Please provide 1 ~ 9 \n"); 
    }
    return len;
}
/*
** Module Init function
*/
static int __init etx_driver_init(void) {
/*Allocating Major number*/
    if((alloc_chrdev_region(&dev, 0, 9, "etx_Dev")) <0){
        pr_err("Cannot allocate major number\n");
        goto r_unreg;
    }
    pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
/*Creating cdev structure*/
    cdev_init(&etx_cdev,&fops);
/*Adding character device to the system*/
    if((cdev_add(&etx_cdev,dev,10)) < 0){
        pr_err("Cannot add the device to the system\n");
        goto r_del;
    }
/*Creating struct class*/
    if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL){
        pr_err("Cannot create the struct class\n");
        goto r_class;
    }
/*Creating device*/
    if((device_create(dev_class,NULL,dev,NULL,"etx_device")) == NULL){
        pr_err( "Cannot create the Device \n");
    goto r_device;
    }
//Checking the GPIO 21,18,15,14 is valid or not
    if(gpio_is_valid(GPIO_21) == false){
        pr_err("GPIO %d is not valid\n", GPIO_21);
        goto r_device;
    }
    if(gpio_is_valid(GPIO_20) == false){
        pr_err("GPIO %d is not valid\n", GPIO_20);
        goto r_device;
    }
    if(gpio_is_valid(GPIO_16) == false){
        pr_err("GPIO %d is not valid\n", GPIO_16);
        goto r_device;
    }
    if(gpio_is_valid(GPIO_12) == false){
        pr_err("GPIO %d is not valid\n", GPIO_12);
        goto r_device;
    }
    if(gpio_is_valid(GPIO_18) == false){
        pr_err("GPIO %d is not valid\n", GPIO_18);
        goto r_device;
    }
    if(gpio_is_valid(GPIO_15) == false){
        pr_err("GPIO %d is not valid\n", GPIO_15);
        goto r_device;
    }
    if(gpio_is_valid(GPIO_14) == false){
        pr_err("GPIO %d is not valid\n", GPIO_14);
        goto r_device;
    }
    if(gpio_is_valid(GPIO_4) == false){
        pr_err("GPIO %d is not valid\n", GPIO_4);
        goto r_device;
    }
    if(gpio_is_valid(GPIO_27) == false){
        pr_err("GPIO %d is not valid\n", GPIO_27);
        goto r_device;
    }
    if(gpio_is_valid(GPIO_22) == false){
        pr_err("GPIO %d is not valid\n", GPIO_22);
        goto r_device;
    }
//Requesting the GPIO 21,18,15,14
    if(gpio_request(GPIO_21,"GPIO_21") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_21);
        goto r_gpio;
    }
    if(gpio_request(GPIO_20,"GPIO_20") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_20);
        goto r_gpio;
    }
    if(gpio_request(GPIO_16,"GPIO_16") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_16);
        goto r_gpio;
    }
    if(gpio_request(GPIO_12,"GPIO_12") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_12);
        goto r_gpio;
    }
    if(gpio_request(GPIO_18,"GPIO_18") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_18);
        goto r_gpio;
    }
    if(gpio_request(GPIO_15,"GPIO_15") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_15);
        goto r_gpio;
    }
    if(gpio_request(GPIO_14,"GPIO_14") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_14);
        goto r_gpio;
    }
    if(gpio_request(GPIO_4,"GPIO_4") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_4);
        goto r_gpio;
    }
    if(gpio_request(GPIO_27,"GPIO_27") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_27);
        goto r_gpio;
    }
    if(gpio_request(GPIO_22,"GPIO_22") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_22);
        goto r_gpio;
    }
//configure the GPIO 21 as output
    gpio_direction_output(GPIO_21, 0);
    gpio_direction_output(GPIO_20, 0);
    gpio_direction_output(GPIO_16, 0);
    gpio_direction_output(GPIO_12, 0);
    gpio_direction_output(GPIO_18, 0);
    gpio_direction_output(GPIO_15, 0);
    gpio_direction_output(GPIO_14, 0);
    gpio_direction_output(GPIO_4, 0);
    gpio_direction_output(GPIO_27, 0);
    gpio_direction_output(GPIO_22, 0);
    
/* Using this call the GPIO 21 will be visible in /sys/class/gpio/
** Now you can change the gpio values by using below commands also.
** echo 1 > /sys/class/gpio/gpio21/value (turn ON the LED)
** echo 0 > /sys/class/gpio/gpio21/value (turn OFF the LED)
** cat /sys/class/gpio/gpio21/value (read the value LED)
**
** the second argument prevents the direction from being changed.
*/
    gpio_export(GPIO_21, false);
    gpio_export(GPIO_20, false);
    gpio_export(GPIO_16, false);
    gpio_export(GPIO_12, false);
    gpio_export(GPIO_18, false);
    gpio_export(GPIO_15, false);
    gpio_export(GPIO_14, false);
    gpio_export(GPIO_4, false);
    gpio_export(GPIO_27, false);
    gpio_export(GPIO_22, false);
    pr_info("Device Driver Insert...Done!!!\n");
    return 0;
    r_gpio:
        gpio_free(GPIO_21);
        gpio_free(GPIO_20);
        gpio_free(GPIO_16);
        gpio_free(GPIO_12);
        gpio_free(GPIO_18);
        gpio_free(GPIO_15);
        gpio_free(GPIO_14);
        gpio_free(GPIO_4);
        gpio_free(GPIO_27);
        gpio_free(GPIO_22);
    r_device:
        device_destroy(dev_class,dev);
    r_class:
        class_destroy(dev_class);
    r_del:
        cdev_del(&etx_cdev);
    r_unreg:
        unregister_chrdev_region(dev,10);
    return -1;
}
/*
** Module exit function
*/
static void __exit etx_driver_exit(void) {
    gpio_unexport(GPIO_21);
    gpio_unexport(GPIO_18);
    gpio_unexport(GPIO_15);
    gpio_unexport(GPIO_14);
    gpio_unexport(GPIO_20);
    gpio_unexport(GPIO_16);
    gpio_unexport(GPIO_12);
    gpio_unexport(GPIO_4);
    gpio_unexport(GPIO_27);
    gpio_unexport(GPIO_22);

    gpio_free(GPIO_21);
    gpio_free(GPIO_18);
    gpio_free(GPIO_15);
    gpio_free(GPIO_14);
    gpio_free(GPIO_20);
    gpio_free(GPIO_16);
    gpio_free(GPIO_12);
    gpio_free(GPIO_4);
    gpio_free(GPIO_27);
    gpio_free(GPIO_22);

    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    cdev_del(&etx_cdev);
    unregister_chrdev_region(dev, 10);
    pr_info("Device Driver Remove...Done!!\n");
}
module_init(etx_driver_init);
module_exit(etx_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <embetronicx@gmail.com>");
MODULE_DESCRIPTION("A simple device driver - GPIO Driver");
MODULE_VERSION("1.32");