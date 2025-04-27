/*
 * cdev.c - 字符设备驱动示例模块
 * 演示如何创建和注册一个字符设备，包括设备号的分配和cdev的初始化
 */

/* 包含必要的头文件 */
#include<linux/module.h>    // 提供模块相关的功能
#include<linux/init.h>      // 提供__init和__exit宏
#include<linux/fs.h>        // 提供文件系统相关函数
#include<linux/kdev_t.h>    // 提供设备号相关的宏和函数
#include<linux/cdev.h>      // 提供字符设备相关的功能

/* 定义全局变量 */
static dev_t dev_num;       // 设备号
struct cdev cdev_test;      // 字符设备结构体

/**
 * 文件操作结构体
 * 定义设备支持的操作函数
 * 这里只设置了owner，实际应用中需要添加read、write等操作
 */
struct file_operations cdev_test_ops = {
	.owner = THIS_MODULE    // 表明这个设备归属于此模块
};

/**
 * 模块初始化函数
 * 完成设备号申请和字符设备的注册
 */
static int __init module_cdev_init(void)
{
	int ret;
	int major, minor;
	
	/* 动态申请设备号 */
	ret = alloc_chrdev_region(&dev_num, 0, 1, "chrdev_name");
	if(ret < 0) {
		printk("alloc_chrdev_region is error\n");
		return ret;
	}
	printk("alloc_chrdev_region is ok\n");
	
	/* 获取主次设备号 */
	major = MAJOR(dev_num);
	minor = MINOR(dev_num);    
	printk("major is %d\n", major);
	printk("minor is %d\n", minor);

	/* 初始化字符设备结构体 */
	cdev_init(&cdev_test, &cdev_test_ops);  // 初始化cdev结构体
	cdev_test.owner = THIS_MODULE;          // 设置所有者

	/* 添加字符设备到系统 */
	ret = cdev_add(&cdev_test, dev_num, 1);
	if(ret < 0) {
		printk("cdev_add is error\n");
		unregister_chrdev_region(dev_num, 1);  // 添加失败需要释放设备号
		return ret;
	}
	printk("cdev_add is ok\n");

	return 0;
}

/**
 * 模块清理函数
 * 完成字符设备的注销和设备号的释放
 */
static void __exit module_cdev_exit(void)
{
	cdev_del(&cdev_test);                  // 删除字符设备
	unregister_chrdev_region(dev_num, 1);  // 释放设备号
	printk("module exit\n");
}

/* 注册模块的初始化和清理函数 */
module_init(module_cdev_init);    // 注册初始化函数
module_exit(module_cdev_exit);    // 注册清理函数

/* 模块信息 */
MODULE_LICENSE("GPL V2");   // 模块许可证
MODULE_AUTHOR("topeet");    // 模块作者

/*
 * 使用说明：
 * 1. 加载模块：
 *    insmod cdev.ko
 *
 * 2. 查看设备号：
 *    cat /proc/devices
 *    dmesg | tail
 *
 * 3. 卸载模块：
 *    rmmod cdev
 *
 *
*/
