/*
 * 文件名：app.c
 * 描述：字符设备测试应用程序
 * 功能：演示如何通过应用程序访问字符设备
 * 作者：topeet
 */

/* 包含必要的头文件 */
#include<stdio.h>       /* 标准输入输出函数 */
#include<sys/types.h>   /* 基本系统数据类型 */
#include<sys/stat.h>    /* 文件状态信息 */
#include<fcntl.h>       /* 文件控制选项 */
#include<unistd.h>      /* UNIX标准函数 */

/* 主函数 */
int main(int argc, char *argv[])
{
	int fd;                     /* 文件描述符 */
	char buf1[32] = {0};       /* 读取缓冲区 */
	char buf2[32] = "nihao";   /* 写入缓冲区 */

	/* 以读写方式打开设备文件 */
	fd = open("/dev/test", O_RDWR);
	if(fd < 0) {
		perror("open error\n");
		return fd;
	}

	/* 从设备读取数据 */
	read(fd, buf1, sizeof(buf1));
	printf("buf1 is %s\r\n", buf1);

	/* 向设备写入数据 */
	write(fd, buf2, sizeof(buf2));

	/* 关闭设备文件 */
	close(fd);

	return 0;
}
