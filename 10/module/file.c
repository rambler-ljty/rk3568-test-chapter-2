/*
 * 文件名：file.c
 * 描述：Linux字符设备驱动示例程序（改进版）
 * 功能：演示如何使用私有数据结构的字符设备驱动
 * 作者：topeet
 */

/* 包含必要的头文件 */
#include<linux/module.h>    /* 提供模块相关的功能 */
#include<linux/init.h>      /* 提供模块初始化和退出相关的宏 */
#include<linux/fs.h>        /* 提供文件操作相关的结构体和函数 */
#include<linux/kdev_t.h>    /* 提供设备号相关的宏和函数 */
#include<linux/cdev.h>      /* 提供字符设备相关的结构体和函数 */
#include<linux/uaccess.h>   /* 提供用户空间和内核空间数据传输的函数 */

/* 定义设备私有数据结构 */
struct device_test {
	dev_t dev_num;              /* 设备号 */
	int major;                  /* 主设备号 */
	int minor;                  /* 次设备号 */
	struct cdev cdev_test;      /* 字符设备结构体 */
	struct class *class;        /* 设备类 */
	struct device *device;      /* 设备 */
	char kbuf[32];             /* 内核缓冲区 */
};

/* 声明全局设备结构体变量 */
struct device_test dev1;

/* 设备打开函数 */
static int cdev_test_open(struct inode *inode, struct file *file)
{
	/* 将设备结构体保存在文件私有数据中 */
	file->private_data = &dev1;
	printk("This is cdev_test_open\n");
	return 0;
}

/* 设备写入函数 */
static ssize_t cdev_test_write(struct file *file, const char __user *buf, size_t size, loff_t *off)
{
	/* 获取设备结构体指针 */
	struct device_test *test_dev = (struct device_test *)file->private_data;

	/* 从用户空间复制数据到设备的内核缓冲区 */
	if(copy_from_user(test_dev->kbuf, buf, size) != 0)
	{
		printk("copy_from_user error\n");
		return -1;
	}
	printk("This is cdev_test_write\n");
	printk("kbuf is %s\n", test_dev->kbuf);
	return 0;
}

/* 设备读取函数 */
static ssize_t cdev_test_read(struct file *file, char __user *buf, size_t size, loff_t *off)
{
	/* 获取设备结构体指针 */
	struct device_test *test_dev = (struct device_test *)file->private_data;
	
	/* 将设备的内核缓冲区数据复制到用户空间 */
	if(copy_to_user(buf, test_dev->kbuf, strlen(test_dev->kbuf)) != 0)
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
	ret = alloc_chrdev_region(&dev1.dev_num, 0, 1, "alloc_name");
	if(ret < 0) {
		printk("alloc_chrdev_region is error\n");
	}
	printk("alloc_chrdev_region is ok\n");
	
	/* 获取主次设备号 */
	dev1.major = MAJOR(dev1.dev_num);
	dev1.minor = MINOR(dev1.dev_num);    
	printk("major is %d\n", dev1.major);
	printk("minor is %d\n", dev1.minor);

	/* 初始化并添加字符设备 */
	dev1.cdev_test.owner = THIS_MODULE;
	cdev_init(&dev1.cdev_test, &cdev_test_fops);
	ret = cdev_add(&dev1.cdev_test, dev1.dev_num, 1);
	if(ret < 0) {
		printk("cdev_add is error\n");
	}
	printk("cdev_add is ok\n");
	
	/* 创建设备类和设备节点 */
	dev1.class = class_create(THIS_MODULE, "test");
	device_create(dev1.class, NULL, dev1.dev_num, NULL, "test");

	return 0;
}

/* 模块退出函数 */
static void __exit chr_fops_exit(void)
{
	unregister_chrdev_region(dev1.dev_num, 1);  /* 注销设备号 */
	cdev_del(&dev1.cdev_test);                  /* 删除字符设备 */
	device_destroy(dev1.class, dev1.dev_num);   /* 销毁设备节点 */
	class_destroy(dev1.class);                  /* 销毁设备类 */
	printk("module exit\n");
}

/* 声明模块的入口和出口函数 */
module_init(chr_fops_init);
module_exit(chr_fops_exit);

/* 模块许可证声明 */
MODULE_LICENSE("GPL v2");
/* 模块作者声明 */
MODULE_AUTHOR("topeet");
