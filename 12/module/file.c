/*
 * 文件名：file.c
 * 描述：Linux字符设备驱动示例程序（错误处理版本）
 * 功能：演示字符设备驱动中的错误处理机制
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
	file->private_data = &dev1;  /* 保存设备结构体指针 */
	printk("This is cdev_test_open\n");
	return 0;
}

/* 设备写入函数 */
static ssize_t cdev_test_write(struct file *file, const char __user *buf, size_t size, loff_t *off)
{
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
		goto err_chrdev;  /* 分配失败，跳转到错误处理 */
	}
	printk("alloc_chrdev_region is ok\n");
	
	/* 获取主次设备号 */
	dev1.major = MAJOR(dev1.dev_num);
	dev1.minor = MINOR(dev1.dev_num);    
	printk("major is %d\n", dev1.major);
	printk("minor is %d\n", dev1.minor);

	/* 初始化字符设备 */
	dev1.cdev_test.owner = THIS_MODULE;
	cdev_init(&dev1.cdev_test, &cdev_test_fops);

	/* 添加字符设备 */
	ret = cdev_add(&dev1.cdev_test, dev1.dev_num, 1);
	if(ret < 0) {
		goto err_chr_add;  /* 添加失败，跳转到错误处理 */
	}

	/* 创建设备类 */
	dev1.class = class_create(THIS_MODULE, "test");
	if(IS_ERR(dev1.class)) {
		ret = PTR_ERR(dev1.class);
		goto err_class_create;  /* 创建失败，跳转到错误处理 */
	}

	/* 创建设备节点 */
	dev1.device = device_create(dev1.class, NULL, dev1.dev_num, NULL, "test");
	if(IS_ERR(dev1.device)) {
		ret = PTR_ERR(dev1.device);
		goto err_device_create;  /* 创建失败，跳转到错误处理 */
	}

	return 0;

	/* 错误处理代码块 */
err_device_create:
	class_destroy(dev1.class);
err_class_create:
	cdev_del(&dev1.cdev_test);
err_chr_add:
	unregister_chrdev_region(dev1.dev_num, 1);
err_chrdev:
	return ret;
}

/* 模块退出函数 */
static void __exit chr_fops_exit(void)
{
	device_destroy(dev1.class, dev1.dev_num);   /* 销毁设备节点 */
	class_destroy(dev1.class);                  /* 销毁设备类 */
	cdev_del(&dev1.cdev_test);                  /* 删除字符设备 */
	unregister_chrdev_region(dev1.dev_num, 1);  /* 注销设备号 */
	printk("module exit\n");
}

/* 声明模块的入口和出口函数 */
module_init(chr_fops_init);
module_exit(chr_fops_exit);

/* 模块许可证声明 */
MODULE_LICENSE("GPL v2");
/* 模块作者声明 */
MODULE_AUTHOR("topeet");
