/*
 * app.c - 字符设备驱动测试程序
 * 用于测试字符设备的读写操作
 */

/* 包含必要的头文件 */
#include<stdio.h>       // 标准输入输出函数
#include<stdlib.h>      // 标准库函数
#include<sys/types.h>   // 基本系统数据类型
#include<sys/stat.h>    // 文件状态相关
#include<fcntl.h>      // 文件控制选项
#include<unistd.h>     // UNIX标准函数
#include<string.h>     // 字符串处理函数

/**
 * 主函数
 * @argc: 命令行参数个数
 * @argv: 命令行参数数组
 *        argv[1]: 设备文件路径
 *        argv[2]: 操作类型（"read"或"write"）
 * @return: 成功返回0，失败返回-1
 */
int main(int argc, char *argv[])
{
	int fd;                     // 文件描述符
	char buf[32];              // 读写缓冲区

	/* 检查参数个数 */
	if (argc != 3) {
		printf("Usage: %s <device> <read|write>\n", argv[0]);
		return -1;
	}

	/* 打开设备文件 */
	fd = open(argv[1], O_RDWR, 0666);  // 以读写方式打开
	if(fd < 0) {
		printf("open is error\n");
		return -1;
	}
	printf("open is ok\n");

	/* 根据命令行参数执行相应操作 */
	if(!strcmp(argv[2], "read")) {
		/* 读取设备数据 */
		read(fd, buf, 32);
		printf("Read from device: %s\n", buf);  // 打印读取的数据
	}
	else if(!strcmp(argv[2], "write")) {
		/* 向设备写入数据 */
		write(fd, "hello\n", 6);
		printf("Write to device: hello\n");
	}
	else {
		printf("Invalid operation: %s\n", argv[2]);
		close(fd);
		return -1;
	}

	/* 关闭设备文件 */
	close(fd);
	return 0;
}

/*
 * 使用说明：
 * 1. 编译：
 *    aarch64-linux-gnu-gcc -o app app.c
 *
 * 2. 读取设备：
 *    ./app /dev/device_test read
 *
 * 3. 写入设备：
 *    ./app /dev/device_test write
 *
  */
