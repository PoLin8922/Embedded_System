#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xbc2a14bb, "module_layout" },
	{ 0xcdd727b7, "gpiod_unexport" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x69a9eb47, "cdev_del" },
	{ 0xacc3f603, "class_destroy" },
	{ 0xd1a9648b, "device_destroy" },
	{ 0x75755bbd, "gpiod_export" },
	{ 0x6acdc829, "gpiod_direction_output_raw" },
	{ 0xfe990052, "gpio_free" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x696f7ea8, "device_create" },
	{ 0x4c3051a8, "__class_create" },
	{ 0x796c26ba, "cdev_add" },
	{ 0xfeb3f1f3, "cdev_init" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x8da6585d, "__stack_chk_fail" },
	{ 0x2d9e87b7, "gpiod_set_raw_value" },
	{ 0x56470118, "__warn_printk" },
	{ 0xdcb764ad, "memset" },
	{ 0x12a4e128, "__arch_copy_from_user" },
	{ 0x5d5620c3, "gpiod_get_raw_value" },
	{ 0x4953a3db, "gpio_to_desc" },
	{ 0x92997ed8, "_printk" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "04CDBD9AF17C94065AFA331");
