/*
 * 文件名：miscdevice.c
 * 描述：Linux杂项设备驱动示例程序
 * 功能：演示如何注册和注销一个杂项设备
 * 作者：topeet
 */

#include<linux/module.h>    /* 提供模块相关的功能 */
#include<linux/init.h>      /* 提供模块初始化和退出相关的宏 */
#include<linux/fs.h>        /* 提供文件操作相关的结构体和函数 */
#include<linux/miscdevice.h> /* 提供杂项设备相关的定义和函数 */

/* 定义文件操作结构体，这里只设置了owner字段 */
struct file_operations misc_fops = {
	.owner = THIS_MODULE    /* 表明这个设备驱动归属于当前模块 */
};

/* 定义杂项设备结构体 */
struct miscdevice misc_dev = {
	.minor = MISC_DYNAMIC_MINOR,  /* 动态分配次设备号 */
	.name = "test",               /* 设备名称，将在/dev下创建同名设备节点 */
	.fops = &misc_fops           /* 指向文件操作结构体 */
};

/*
 * 模块初始化函数
 * 返回值：成功返回0，失败返回负值
 */
static int __init misc_init(void)
{
	int ret;
	/* 注册杂项设备 */
	ret = misc_register(&misc_dev);
	if(ret < 0) {
		printk("misc register is error\n");  /* 注册失败时打印错误信息 */
	}
	printk("misc register is ok\n");         /* 注册成功时打印信息 */

	return 0;
}

/*
 * 模块退出函数
 * 功能：注销杂项设备并清理资源
 */
static void __exit misc_exit(void)
{
	misc_deregister(&misc_dev);    /* 注销杂项设备 */
	printk("misc exit!\n");        /* 打印退出信息 */
}

/* 声明模块的入口和出口函数 */
module_init(misc_init);
module_exit(misc_exit);

/* 模块许可证声明 */
MODULE_LICENSE("GPL V2");
/* 模块作者声明 */
MODULE_AUTHOR("topeet");
