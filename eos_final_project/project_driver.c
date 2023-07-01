#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/uaccess.h> //copy_to/from_user()
#include <linux/gpio.h> //GPIO

//led is connected to this GPIO
#define GPIO_0 (0)
#define GPIO_1 (1)
//#define GPIO_2 (2)
#define GPIO_3 (3)
#define GPIO_4 (4)
#define GPIO_5 (5)
#define GPIO_6 (6)
#define GPIO_7 (7) 
#define GPIO_8 (8)
#define GPIO_9 (9)

//button is connected to this GPIO
#define GPIO_10 (10)	// button_00
#define GPIO_11 (11)	// button_01
#define GPIO_12 (12)	// button_02
//7segment is connected to this GPIO
#define GPIO_13 (13)
#define GPIO_14 (14)
#define GPIO_15 (15)
#define GPIO_16 (16)
#define GPIO_17 (17)
#define GPIO_18 (18)
#define GPIO_19 (19) 
#define GPIO_20 (20)
#define GPIO_21 (21)
#define GPIO_22 (22)
#define GPIO_23 (23)
#define GPIO_24 (24)
#define GPIO_25 (25)
#define GPIO_26 (26)
#define GPIO_27 (27)


#define BUF_SIZE 10
// 5 file,3 for button,1 for 7segment,1 for led
static struct gpio leds[] = {
	// button * 3 [0~2]
	{  GPIO_6, GPIOF_IN, "button_00" },
	{  GPIO_26, GPIOF_IN, "button_01" },
	{  GPIO_13, GPIOF_IN, "button_02" },
	// 7segment * 7 [3~9]
	{  GPIO_17, GPIOF_OUT_INIT_HIGH, "seg" }, //7-g
	{  GPIO_27, GPIOF_OUT_INIT_HIGH, "seg" }, //7-f
	{  GPIO_22, GPIOF_OUT_INIT_HIGH, "seg" }, //7-a
	{  GPIO_10, GPIOF_OUT_INIT_HIGH, "seg" }, //7-b
	{  GPIO_9, GPIOF_OUT_INIT_HIGH, "seg" }, //7-e
	{  GPIO_11, GPIOF_OUT_INIT_HIGH, "seg" }, //7-d
	{  GPIO_5, GPIOF_OUT_INIT_HIGH, "seg" }, //7-c
	// led * 12 [10~21]
	{  GPIO_14, GPIOF_OUT_INIT_LOW, "led" }, // A
	{  GPIO_15, GPIOF_OUT_INIT_LOW, "led" }, // B
	{  GPIO_18, GPIOF_OUT_INIT_LOW, "led" }, // C
	{  GPIO_23, GPIOF_OUT_INIT_LOW, "led" }, // D
	{  GPIO_24, GPIOF_OUT_INIT_LOW, "led" }, // E
	{  GPIO_25, GPIOF_OUT_INIT_LOW, "led" }, // F
	{  GPIO_8, GPIOF_OUT_INIT_LOW, "led" }, // G 
	{  GPIO_7, GPIOF_OUT_INIT_LOW, "led" }, // H
	{  GPIO_1, GPIOF_OUT_INIT_LOW, "led" }, // I
	{  GPIO_12, GPIOF_OUT_INIT_LOW, "led" }, // J
	{  GPIO_16, GPIOF_OUT_INIT_LOW, "led" }, // K
	{  GPIO_20, GPIOF_OUT_INIT_LOW, "led" } // L
};

dev_t deviceNO = 0;
static struct class *dev_class;
static struct cdev etx_cdev;

static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);

/*************** Driver functions **********************/
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp, const char *buf, size_t len, loff_t * off);
/******************************************************/

//File operation structure
static struct file_operations fops =
{
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
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	uint8_t gpio_state = 0;

	//get specific GPIO to be read
	unsigned int gpio = iminor(filp->f_path.dentry->d_inode);

	//reading GPIO value
	gpio_state = gpio_get_value(leds[gpio].gpio);
	len = 1;
	//write to user
	if(copy_to_user(buf, &gpio_state, len) > 0) {
		pr_err("ERROR: Not all the bytes have been copied to user\n");
	}
	pr_info("Read function : GPIO = %d \n", gpio_state);

	return 0;
}

/*
 ** This function will be called when we write the Device file
 */
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	uint8_t rec_buf[BUF_SIZE] = {0};

	//get specific GPIO to be written
	unsigned int gpio = iminor(filp->f_path.dentry->d_inode);
	unsigned int length = len < BUF_SIZE ? len-1 : BUF_SIZE-1;
	if(copy_from_user(rec_buf, buf, length) > 0) {
		pr_err("ERROR: Not all the bytes have been copied from user\n");
	}
	//receive system call number to write on Rpi

	// //[e] setup gpio bits
	// if(rec_buf[0] == 0 || rec_buf[0] == 1){
	// 	gpio_set_value(leds[gpio].gpio, rec_buf[0]);
	// }else{
	// 	pr_err("ERROR: recieving value is neithor 0 or 1\n");
	// }

	/* 7segment GPIO */
	if (gpio == 0) {
		if(rec_buf[0] == 0 || rec_buf[0] == 1)
		{
		 	gpio_set_value(leds[gpio].gpio, rec_buf[0]);
		} 	
	}
	else if (gpio == 1) {
		if(rec_buf[0] == 0 || rec_buf[0] == 1)
		{
		 	gpio_set_value(leds[gpio].gpio, rec_buf[0]);
		} 	
	}
	else if (gpio == 2) {
		if(rec_buf[0] == 0 || rec_buf[0] == 1)
		{
		 	gpio_set_value(leds[gpio].gpio, rec_buf[0]);
		} 	
	}
	else if (rec_buf[0]=='0') {
		gpio_set_value(GPIO_17, 1);
		gpio_set_value(GPIO_27, 0);
		gpio_set_value(GPIO_22, 0);
		gpio_set_value(GPIO_10, 0);
		gpio_set_value(GPIO_9, 0);
		gpio_set_value(GPIO_11, 0);
		gpio_set_value(GPIO_5, 0);
	}
	else if (rec_buf[0]=='1') {
		gpio_set_value(GPIO_17, 1);
		gpio_set_value(GPIO_27, 1);
		gpio_set_value(GPIO_22, 1);
		gpio_set_value(GPIO_10, 0);
		gpio_set_value(GPIO_9, 1);
		gpio_set_value(GPIO_11, 1);
		gpio_set_value(GPIO_5, 0);
	}
	else if (rec_buf[0]=='2') {
		gpio_set_value(GPIO_17, 0);
		gpio_set_value(GPIO_27, 1);
		gpio_set_value(GPIO_22, 0);
		gpio_set_value(GPIO_10, 0);
		gpio_set_value(GPIO_9, 0);
		gpio_set_value(GPIO_11, 0);
		gpio_set_value(GPIO_5, 1);
	}
	else if (rec_buf[0]=='3') {
		gpio_set_value(GPIO_17, 0);
		gpio_set_value(GPIO_27, 1);
		gpio_set_value(GPIO_22, 0);
		gpio_set_value(GPIO_10, 0);
		gpio_set_value(GPIO_9, 1);
		gpio_set_value(GPIO_11, 0);
		gpio_set_value(GPIO_5, 0);
	}
	else if (rec_buf[0]=='4') {
		gpio_set_value(GPIO_17, 0);
		gpio_set_value(GPIO_27, 0);
		gpio_set_value(GPIO_22, 1);
		gpio_set_value(GPIO_10, 0);
		gpio_set_value(GPIO_9, 1);
		gpio_set_value(GPIO_11, 1);
		gpio_set_value(GPIO_5, 0);
	}
	else if (rec_buf[0]=='5') {
		gpio_set_value(GPIO_17, 0);
		gpio_set_value(GPIO_27, 0);
		gpio_set_value(GPIO_22, 0);
		gpio_set_value(GPIO_10, 1);
		gpio_set_value(GPIO_9, 1);
		gpio_set_value(GPIO_11, 0);
		gpio_set_value(GPIO_5, 0);
	}
	else if (rec_buf[0]=='6') {
		gpio_set_value(GPIO_17, 0);
		gpio_set_value(GPIO_27, 0);
		gpio_set_value(GPIO_22, 0);
		gpio_set_value(GPIO_10, 1);
		gpio_set_value(GPIO_9, 0);
		gpio_set_value(GPIO_11, 0);
		gpio_set_value(GPIO_5, 0);
	}
	else if (rec_buf[0]=='7') {
		gpio_set_value(GPIO_17, 1);
		gpio_set_value(GPIO_27, 1);
		gpio_set_value(GPIO_22, 0);
		gpio_set_value(GPIO_10, 0);
		gpio_set_value(GPIO_9, 1);
		gpio_set_value(GPIO_11, 1);
		gpio_set_value(GPIO_5, 0);
	}
	else if (rec_buf[0]=='8') {
		gpio_set_value(GPIO_17, 0);
		gpio_set_value(GPIO_27, 0);
		gpio_set_value(GPIO_22, 0);
		gpio_set_value(GPIO_10, 0);
		gpio_set_value(GPIO_9, 0);
		gpio_set_value(GPIO_11, 0);
		gpio_set_value(GPIO_5, 0);
	}
	else if (rec_buf[0]=='9') {
		gpio_set_value(GPIO_17, 0);
		gpio_set_value(GPIO_27, 0);
		gpio_set_value(GPIO_22, 0);
		gpio_set_value(GPIO_10, 0);
		gpio_set_value(GPIO_9, 1);
		gpio_set_value(GPIO_11, 1);
		gpio_set_value(GPIO_5, 0);
	}
	else if (rec_buf[0]=='A') {
		gpio_set_value(GPIO_17, 0);
		gpio_set_value(GPIO_27, 0);
		gpio_set_value(GPIO_22, 0);
		gpio_set_value(GPIO_10, 0);
		gpio_set_value(GPIO_9, 0);
		gpio_set_value(GPIO_11, 1);
		gpio_set_value(GPIO_5, 0);
	}
	else if (rec_buf[0]=='B') {
		gpio_set_value(GPIO_17, 0);
		gpio_set_value(GPIO_27, 0);
		gpio_set_value(GPIO_22, 1);
		gpio_set_value(GPIO_10, 1);
		gpio_set_value(GPIO_9, 0);
		gpio_set_value(GPIO_11, 0);
		gpio_set_value(GPIO_5, 0);
	}
	else if (rec_buf[0]=='C') {
		gpio_set_value(GPIO_17, 1);
		gpio_set_value(GPIO_27, 0);
		gpio_set_value(GPIO_22, 0);
		gpio_set_value(GPIO_10, 1);
		gpio_set_value(GPIO_9, 0);
		gpio_set_value(GPIO_11, 0);
		gpio_set_value(GPIO_5, 1);
	}
	else if (rec_buf[0]=='x') {// all off
		gpio_set_value(GPIO_17, 1);
		gpio_set_value(GPIO_27, 1);
		gpio_set_value(GPIO_22, 1);
		gpio_set_value(GPIO_10, 1);
		gpio_set_value(GPIO_9, 1);
		gpio_set_value(GPIO_11, 1);
		gpio_set_value(GPIO_5, 1);
	}/* LED GPIO*/
	else if (rec_buf[0]=='z') {// all off 
		gpio_set_value(GPIO_14, 0);
		gpio_set_value(GPIO_15, 0);
		gpio_set_value(GPIO_18, 0);
		gpio_set_value(GPIO_23, 0);
		gpio_set_value(GPIO_24, 0);
		gpio_set_value(GPIO_25, 0);
		gpio_set_value(GPIO_8, 0);
		gpio_set_value(GPIO_7, 0);
		gpio_set_value(GPIO_1, 0);
		gpio_set_value(GPIO_12, 0);
		gpio_set_value(GPIO_16, 0);
		gpio_set_value(GPIO_20, 0);
	}
	else if (rec_buf[0]=='a') { // Section A on
		gpio_set_value(GPIO_14, 1);
	}
	else if (rec_buf[0]=='m') { // Section A off
		gpio_set_value(GPIO_14, 0);
	}
	else if (rec_buf[0]=='b') { // Section B on
		gpio_set_value(GPIO_15, 1);
	}
	else if (rec_buf[0]=='n') { // Section B off
		gpio_set_value(GPIO_15, 0);
	}
	else if (rec_buf[0]=='c') { // Section C on
		gpio_set_value(GPIO_18, 1);
	}
	else if (rec_buf[0]=='o') { // Section C off
		gpio_set_value(GPIO_18, 0);
	}
	else if (rec_buf[0]=='d') { // Section D on
		gpio_set_value(GPIO_23, 1);
	}
	else if (rec_buf[0]=='p') { // Section D off
		gpio_set_value(GPIO_23, 0);
	}
	else if (rec_buf[0]=='e') { // Section E on
		gpio_set_value(GPIO_24, 1);
	}
	else if (rec_buf[0]=='q') { // Section E off
		gpio_set_value(GPIO_24, 0);
	}
	else if (rec_buf[0]=='f') { // Section F on
		gpio_set_value(GPIO_25, 1);
	}
	else if (rec_buf[0]=='r') { // Section F off
		gpio_set_value(GPIO_25, 0);
	}
	else if (rec_buf[0]=='g') { // Section G on
		gpio_set_value(GPIO_8, 1);
	}
	else if (rec_buf[0]=='s') { // Section G off
		gpio_set_value(GPIO_8, 0);
	}
	else if (rec_buf[0]=='h') { // Section H on
		gpio_set_value(GPIO_7, 1);
	}
	else if (rec_buf[0]=='t') { // Section H off
		gpio_set_value(GPIO_7, 0);
	}
	else if (rec_buf[0]=='i') { // Section I on
		gpio_set_value(GPIO_1, 1);
	}
	else if (rec_buf[0]=='u') { // Section I off
		gpio_set_value(GPIO_1, 0);
	}
	else if (rec_buf[0]=='j') { // Section J on
		gpio_set_value(GPIO_12, 1);
	}
	else if (rec_buf[0]=='v') { // Section J off
		gpio_set_value(GPIO_12, 0);
	}
	else if (rec_buf[0]=='k') { // Section K on
		gpio_set_value(GPIO_16, 1);
	}
	else if (rec_buf[0]=='w') { // Section K off
		gpio_set_value(GPIO_16, 0);
	}
	else if (rec_buf[0]=='l') { // Section L on
		gpio_set_value(GPIO_20, 1);
	}
	else if (rec_buf[0]=='y') { // Section L off
		gpio_set_value(GPIO_20, 0);
	}
	else 
	{
		pr_err("Unknown command : Please provide either 1 or 0 \n"); 
	}
	pr_info("Complete writing...!!!\n");
	return len;
}

/*
 ** Module Init function
 */
static int __init etx_driver_init(void)
{
	int i;

	/*Allocating Major number*/
	if((alloc_chrdev_region(&deviceNO, 0, ARRAY_SIZE(leds), "etx_Dev")) <0){
		pr_err("Cannot allocate major number\n");
		goto r_unreg;
	}
	pr_info("Major = %d Minor = %d \n", MAJOR(deviceNO), MINOR(deviceNO));

	/*Creating cdev structure*/
	cdev_init(&etx_cdev, &fops);

	/*Adding character device to the system*/
	if((cdev_add(&etx_cdev, deviceNO, ARRAY_SIZE(leds))) < 0){
		pr_err("Cannot add the device to the system\n");
		goto r_del;
	}

	/*Creating struct class*/
	if((dev_class = class_create(THIS_MODULE, "etx_class")) == NULL){
		pr_err("Cannot create the struct class\n");
		goto r_class;
	}

	/*Creating device*/
	for (i = 0; i < ARRAY_SIZE(leds); ++i) {
		if (device_create(dev_class, NULL, MKDEV(MAJOR(deviceNO), MINOR(deviceNO) + i), NULL, leds[i].label) == NULL) {
			pr_err( "Cannot create the Device \n");
			goto r_device;
		}
	}

	//Checking the GPIO is valid or not
	for (i = 0; i < ARRAY_SIZE(leds); ++i) {
		if(gpio_is_valid(leds[i].gpio) == false){
			pr_err("GPIO %d is not valid\n", leds[i].gpio);
			goto r_device;
		}
	}

	//Requesting the GPIO
	if(gpio_request_array(leds, ARRAY_SIZE(leds)) < 0){
		pr_err("ERROR: GPIO array request\n");
		goto r_gpio;
	}

	//configure the GPIO as output
	//gpio_direction_output(GPIO_21, 0);

	/* Using this call the GPIO 21 will be visible in /sys/class/gpio/
	 ** Now you can change the gpio values by using below commands also.
	 ** echo 1 > /sys/class/gpio/gpio21/value (turn ON the LED)
	 ** echo 0 > /sys/class/gpio/gpio21/value (turn OFF the LED)
	 ** cat /sys/class/gpio/gpio21/value (read the value LED)
	 **
	 ** the second argument prevents the direction from being changed.
	 */
	for (i = 0; i < ARRAY_SIZE(leds); ++i)
		gpio_export(leds[i].gpio, false);

	pr_info("Device Driver Insert...Done!!!\n");
	return 0;

r_gpio:
	gpio_free_array(leds, ARRAY_SIZE(leds));
r_device:
	for (i = 0; i < ARRAY_SIZE(leds); i++) {
		gpio_set_value(leds[i].gpio, 0);
		device_destroy(dev_class, MKDEV(MAJOR(deviceNO), MINOR(deviceNO) + i));
	}
r_class:
	class_destroy(dev_class);
r_del:
	cdev_del(&etx_cdev);
r_unreg:
	unregister_chrdev_region(deviceNO, ARRAY_SIZE(leds));

	return -1;
}

/*
 ** Module exit function
 */
static void __exit etx_driver_exit(void)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(leds); ++i) {
		if(i >= 3 && i <= 9)
			gpio_set_value(leds[i].gpio, 1);
		else
			gpio_set_value(leds[i].gpio, 0);
		gpio_unexport(leds[i].gpio);
		device_destroy(dev_class, MKDEV(MAJOR(deviceNO), MINOR(deviceNO) + i));
	}

	class_destroy(dev_class);
	cdev_del(&etx_cdev);
	unregister_chrdev_region(deviceNO, ARRAY_SIZE(leds));
	gpio_free_array(leds, ARRAY_SIZE(leds));
	pr_info("Device Driver Remove...Done!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("");
MODULE_DESCRIPTION("eos_project");
