/*
 * chrdev_fops.c - 字符设备驱动示例
 * 实现基本的字符设备操作，包括open、read、write和release
 */

/* 包含必要的头文件 */
#include<linux/module.h>    // 提供模块相关的功能
#include<linux/init.h>      // 提供__init和__exit宏
#include<linux/fs.h>        // 提供文件系统相关函数
#include<linux/kdev_t.h>    // 提供设备号相关的宏和函数
#include<linux/cdev.h>      // 提供字符设备相关的功能

/**
 * 打开设备函数
 * @inode: 包含设备号等信息
 * @file: 文件结构体
 * @return: 成功返回0
 */
static int chrdev_open(struct inode *inode, struct file *file)
{
	printk("This is chrdev_open\n");
	return 0;
}

/**
 * 读取设备函数
 * @file: 文件结构体
 * @buf: 用户空间缓冲区
 * @size: 要读取的字节数
 * @off: 文件位置指针
 * @return: 实际读取的字节数
 */
static ssize_t chrdev_read(struct file *file, char __user *buf,
                          size_t size, loff_t *off)
{
	printk("This is chrdev_read\n");
	return 0;
}

/**
 * 写入设备函数
 * @file: 文件结构体
 * @buf: 用户空间数据
 * @size: 要写入的字节数
 * @off: 文件位置指针
 * @return: 实际写入的字节数
 */
static ssize_t chrdev_write(struct file *file, const char __user *buf,
                           size_t size, loff_t *off)
{
	printk("This is chrdev_write\n");
	return 0;
}

/**
 * 关闭设备函数
 * @inode: 包含设备号等信息
 * @file: 文件结构体
 * @return: 成功返回0
 */
static int chrdev_release(struct inode *inode, struct file *file)
{
	return 0;
}

/* 定义全局变量 */
static dev_t dev_num;              // 设备号
static struct cdev cdev_test;      // 字符设备结构体

/* 定义文件操作结构体 */
static struct file_operations cdev_fops_test = {
	.owner = THIS_MODULE,          // 模块拥有者
	.open = chrdev_open,           // 打开设备
	.read = chrdev_read,           // 读取设备
	.write = chrdev_write,         // 写入设备
	.release = chrdev_release,     // 关闭设备
};

/* 定义设备类 */
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

	/* 初始化并添加cdev结构体 */
	cdev_init(&cdev_test, &cdev_fops_test);
	cdev_test.owner = THIS_MODULE;

	ret = cdev_add(&cdev_test, dev_num, 1);
	if(ret < 0) {
		printk("cdev_add is error\n");
		unregister_chrdev_region(dev_num, 1);
		return ret;
	}
	printk("cdev_add is ok\n");
	
	/* 创建设备类和设备节点 */
	class_test = class_create(THIS_MODULE, "class_test");
	if (IS_ERR(class_test)) {
		cdev_del(&cdev_test);
		unregister_chrdev_region(dev_num, 1);
		return PTR_ERR(class_test);
	}
	
	if (!device_create(class_test, NULL, dev_num, NULL, "device_test")) {
		class_destroy(class_test);
		cdev_del(&cdev_test);
		unregister_chrdev_region(dev_num, 1);
		return -ENOMEM;
	}

	return 0;
}

/**
 * 模块清理函数
 * 清理所有已分配的资源
 */
static void __exit chrdev_fops_exit(void)
{
	device_destroy(class_test, dev_num);  // 删除设备节点
	class_destroy(class_test);            // 删除设备类
	cdev_del(&cdev_test);                 // 删除字符设备
	unregister_chrdev_region(dev_num, 1); // 释放设备号
	printk("module exit\n");
}

/* 注册模块的初始化和清理函数 */
module_init(chrdev_fops_init);
module_exit(chrdev_fops_exit);

/* 模块信息 */
MODULE_LICENSE("GPL V2");
MODULE_AUTHOR("topeet");

/*
 * 使用说明：
 * 1. 加载模块：
 *    insmod chrdev_fops.ko
 *
 * 2. 查看设备节点：
 *    ls -l /dev/device_test
 *
 * 3. 测试设备操作：
 *    读取test
 *    写入test
 *
 * 4. 卸载模块：
 *    rmmod chrdev_fops
 *
 */
