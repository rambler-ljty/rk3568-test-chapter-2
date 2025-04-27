/*
 * dev_t.c - 字符设备号管理示例模块
 * 演示如何动态或静态申请设备号，以及设备号的管理
 */

/* 包含必要的头文件 */
#include<linux/module.h>    // 提供模块相关的功能
#include<linux/init.h>      // 提供__init和__exit宏
#include<linux/fs.h>        // 提供文件系统相关函数
#include<linux/kdev_t.h>    // 提供设备号相关的宏和函数

/* 定义全局变量 */
static int major;           // 主设备号
static int minor;           // 次设备号

/* 模块参数定义 */
module_param(major, int, S_IRUGO);    // 允许用户加载时指定主设备号
module_param(minor, int, S_IRUGO);    // 允许用户加载时指定次设备号

/* 定义设备号变量 */
static dev_t dev_num;      // dev_t类型变量，用于存储设备号

/**
 * 模块初始化函数
 * 根据用户是否指定主设备号来选择动态或静态分配设备号
 */
static int __init dev_t_init(void)
{
	int ret;

	/* 判断是否指定了主设备号 */
	if(major) {
		/* 静态分配设备号 */
		dev_num = MKDEV(major, minor);    // 根据主次设备号生成设备号
		printk("major is %d\n", major);
		printk("minor is %d\n", minor);
		
		/* 注册设备号 */
		ret = register_chrdev_region(dev_num, 1, "chrdev_name");
		if(ret < 0) {
			printk("register_chrdev_region is error\n");
			return ret;
		}
		printk("register_chrdev_region is ok\n");
	}
	else {
		/* 动态分配设备号 */
		ret = alloc_chrdev_region(&dev_num, 0, 1, "chrdev_name");
		if(ret < 0) {
			printk("alloc_chrdev_region is error\n");
			return ret;
		}
		printk("alloc_chrdev_region is ok\n");
		
		/* 获取分配到的主次设备号 */
		major = MAJOR(dev_num);   // 从设备号中提取主设备号
		minor = MINOR(dev_num);    // 从设备号中提取次设备号
		printk("major is %d\n", major);
		printk("minor is %d\n", minor);
	}

	return 0;
}

/**
 * 模块清理函数
 * 释放申请的设备号
 */
static void __exit dev_t_exit(void)
{
	/* 注销设备号 */
	unregister_chrdev_region(dev_num, 1);
	printk("dev_module exit\n");
}

/* 注册模块的初始化和清理函数 */
module_init(dev_t_init);    // 注册初始化函数
module_exit(dev_t_exit);    // 注册清理函数

/* 模块信息 */
MODULE_LICENSE("GPL V2");   // 模块许可证
MODULE_AUTHOR("topeet");    // 模块作者

/*
 * 使用说明：
 * 1. 静态分配设备号：
 *    insmod dev_t.ko major=200 minor=0
 *
 * 2. 动态分配设备号：
 *    insmod dev_t.ko
 *
 * 3. 查看分配的设备号：
 *    cat /proc/devices
 *    dmesg | tail
 *
 * 注意事项：
 * 1. 主设备号范围：1-255
 * 2. 次设备号范围：0-255
 * 3. 设备号分配失败需要合理处理错误
 * 4. 模块卸载时必须释放设备号
 *
 */
