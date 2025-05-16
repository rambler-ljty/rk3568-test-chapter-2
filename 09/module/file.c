/*
 * 文件名：file.c
 * 描述：Linux字符设备驱动示例程序
 * 功能：演示字符设备的基本操作，包括打开、读取、写入和关闭
 * 作者：topeet
 */

/* 包含必要的头文件 */
#include<linux/module.h>    /* 提供模块相关的功能 */
#include<linux/init.h>      /* 提供模块初始化和退出相关的宏 */
#include<linux/fs.h>        /* 提供文件操作相关的结构体和函数 */
#include<linux/kdev_t.h>    /* 提供设备号相关的宏和函数 */
#include<linux/cdev.h>      /* 提供字符设备相关的结构体和函数 */
#include<linux/uaccess.h>   /* 提供用户空间和内核空间数据传输的函数 */

/* 设备打开函数 */
static int cdev_test_open(struct inode *inode, struct file *file)
{
	printk("This is cdev_test_open\n");
	return 0;
}

/* 设备写入函数 */
static ssize_t cdev_test_write(struct file *file, const char __user *buf, size_t size, loff_t *off)
{
	char kbuf[32] = {0};  /* 内核缓冲区 */
	/* 从用户空间复制数据到内核空间 */
	if(copy_from_user(kbuf, buf, size) != 0)
	{
		printk("copy_from_user error\n");
		return -1;
	}
	printk("This is cdev_test_write\n");
	printk("kbuf is %s\n", kbuf);
	return 0;
}

/* 设备读取函数 */
static ssize_t cdev_test_read(struct file *file, char __user *buf, size_t size, loff_t *off)
{
	char kbuf[32] = "This is cdev_test_read!";  /* 要发送到用户空间的数据 */
	/* 从内核空间复制数据到用户空间 */
	if(copy_to_user(buf, kbuf, strlen(kbuf)) != 0)
	{
		printk("copy_to_user error\n");
		return -1;
	}
	printk("This is cdev_test_read\n");
	return 0;
}

/* 设备关闭函数 */
static int cdev_test_release(struct inode *inode, struct file *file)
{
	printk("This is cdev_test_release\n");
	return 0;
}

/* 全局变量声明 */
static dev_t dev_num;           /* 设备号 */
static int major = 0;           /* 主设备号 */
static int minor = 0;           /* 次设备号 */
static struct cdev cdev_test;   /* 字符设备结构体 */
struct class *class;            /* 设备类 */
struct device *device;          /* 设备 */

/* 文件操作结构体 */
struct file_operations cdev_test_fops = {
	.owner = THIS_MODULE,           /* 模块拥有者 */
	.open = cdev_test_open,         /* 打开操作 */
	.read = cdev_test_read,         /* 读取操作 */
	.write = cdev_test_write,       /* 写入操作 */
	.release = cdev_test_release,   /* 关闭操作 */
};

/* 模块初始化函数 */
static int __init chr_fops_init(void)
{
	int ret;
	
	/* 动态分配设备号 */
	ret = alloc_chrdev_region(&dev_num, 0, 1, "alloc_name");
	if(ret < 0) {
		printk("alloc_chrdev_region is error\n");
	}
	printk("alloc_chrdev_region is ok\n");
	
	/* 获取主次设备号 */
	major = MAJOR(dev_num);
	minor = MINOR(dev_num);    
	printk("major is %d\n", major);
	printk("minor is %d\n", minor);

	/* 初始化字符设备 */
	cdev_init(&cdev_test, &cdev_test_fops);
	cdev_test.owner = THIS_MODULE;

	/* 添加字符设备 */
	ret = cdev_add(&cdev_test, dev_num, 1);
	if(ret < 0) {
		printk("cdev_add is error\n");
	}
	printk("cdev_add is ok\n");
	
	/* 创建设备类 */
	class = class_create(THIS_MODULE, "test");
	/* 创建设备节点 */
	device_create(class, NULL, dev_num, NULL, "test");

	return 0;
}

/* 模块退出函数 */
static void __exit chr_fops_exit(void)
{
	device_destroy(class, dev_num);         /* 销毁设备节点 */
	class_destroy(class);                   /* 销毁设备类 */
	cdev_del(&cdev_test);                   /* 删除字符设备 */
	unregister_chrdev_region(dev_num, 1);  /* 注销设备号 */
	printk("module exit\n");
}

/* 声明模块的入口和出口函数 */
module_init(chr_fops_init);
module_exit(chr_fops_exit);

/* 模块许可证声明 */
MODULE_LICENSE("GPL v2");
/* 模块作者声明 */
MODULE_AUTHOR("topeet");
