/*
 * 文件名：app.c
 * 描述：字符设备测试应用程序
 * 功能：演示如何向字符设备写入数据
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
	char buf1[32] = "nihao";   /* 写入缓冲区 */
	
	/* 以读写方式打开设备文件 */
	fd = open("/dev/test", O_RDWR);
	if(fd < 0) {
		perror("open error\n");
		return fd;
	}

	/* 向设备写入数据 */
	write(fd, buf1, sizeof(buf1));

	/* 关闭设备文件 */
	close(fd);

	return 0;
}
