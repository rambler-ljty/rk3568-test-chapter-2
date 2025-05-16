/*
 * chrdev_node.c - 字符设备节点创建示例模块
 * 演示如何创建字符设备及其对应的设备节点
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
 */
struct file_operations cdev_fops_test = {
	.owner = THIS_MODULE,   // 表明这个设备归属于此模块
};

/* 定义class结构体指针，用于创建设备节点 */
struct class *class_test;

/**
 * 模块初始化函数
 * 完成设备号申请、字符设备注册和设备节点创建
 */
static int __init chrdev_fops_init(void)
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
	cdev_init(&cdev_test, &cdev_fops_test);
	cdev_test.owner = THIS_MODULE;

	/* 添加字符设备到系统 */
	ret = cdev_add(&cdev_test, dev_num, 1);
	if(ret < 0) {
		printk("cdev_add is error\n");
		unregister_chrdev_region(dev_num, 1);  // 失败时释放设备号
		return ret;
	}
	printk("cdev_add is ok\n");
	
	/* 创建设备类 */
	class_test = class_create(THIS_MODULE, "class_test");
	if (IS_ERR(class_test)) {
		printk("class_create failed\n");
		cdev_del(&cdev_test);
		unregister_chrdev_region(dev_num, 1);
		return PTR_ERR(class_test);
	}

	/* 创建设备节点 */
	device_create(class_test, NULL, dev_num, NULL, "device_test");
	// 创建/dev/device_test节点

	return 0;
}

/**
 * 模块清理函数
 * 清理所有已分配的资源
 */
static void __exit chrdev_fops_exit(void)
{
	device_destroy(class_test, dev_num);   // 删除设备节点
	class_destroy(class_test);             // 删除设备类
	cdev_del(&cdev_test);                  // 删除字符设备
	unregister_chrdev_region(dev_num, 1);  // 释放设备号
	printk("module exit\n");
}

/* 注册模块的初始化和清理函数 */
module_init(chrdev_fops_init);    // 注册初始化函数
module_exit(chrdev_fops_exit);    // 注册清理函数

/* 模块信息 */
MODULE_LICENSE("GPL V2");   // 模块许可证
MODULE_AUTHOR("topeet");    // 模块作者

/*
 * 使用说明：
 * 1. 加载模块：
 *    insmod chrdev_node.ko
 *
 * 2. 查看设备节点：
 *    ls -l /dev/device_test
 *
 * 3. 查看设备号：
 *    cat /proc/devices
 *
 * 4. 卸载模块：
 *    rmmod chrdev_node
 */
