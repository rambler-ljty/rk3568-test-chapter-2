/*
 * 文件名：app.c
 * 描述：多设备测试应用程序
 * 功能：演示如何访问多个字符设备
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
	int fd1;                            /* 第一个设备的文件描述符 */
	int fd2;                            /* 第二个设备的文件描述符 */
	char buf1[32] = "nihao /dev/test1"; /* 写入第一个设备的数据 */
	char buf2[32] = "nihao /dev/test2"; /* 写入第二个设备的数据 */

	/* 打开第一个设备 */
	fd1 = open("/dev/test1", O_RDWR);
	if(fd1 < 0) {
		perror("open error\n");
		return fd1;
	}

	/* 向第一个设备写入数据 */
	write(fd1, buf1, sizeof(buf1));

	/* 关闭第一个设备 */
	close(fd1);

	/* 打开第二个设备 */
	fd2 = open("/dev/test2", O_RDWR);
	if(fd2 < 0) {
		perror("open error\n");
		return fd2;
	}

	/* 向第二个设备写入数据 */
	write(fd2, buf2, sizeof(buf2));

	/* 关闭第二个设备 */
	close(fd2);

	return 0;
}
