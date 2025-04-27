/*
 * 文件名：app.c
 * 描述：LED控制测试应用程序
 * 功能：通过命令行参数控制LED的开关状态
 * 作者：topeet
 */

/* 包含必要的头文件 */
#include<stdio.h>       /* 标准输入输出函数 */
#include<sys/types.h>   /* 基本系统数据类型 */
#include<sys/stat.h>    /* 文件状态信息 */
#include<fcntl.h>       /* 文件控制选项 */
#include<unistd.h>      /* UNIX标准函数 */
#include<stdlib.h>      /* 标准库函数 */

/* 主函数 */
int main(int argc, char *argv[])
{
	int fd;                /* 文件描述符 */
	char buf[32] = {0};   /* 数据缓冲区 */
	
	/* 以读写方式打开设备文件 */
	fd = open("/dev/test", O_RDWR);
	if(fd < 0) {
		perror("open error\n");
		return fd;
	}

	/* 将命令行参数转换为整数（0表示关闭LED，1表示打开LED） */
	buf[0] = atoi(argv[1]);

	/* 向设备写入控制命令 */
	write(fd, buf, sizeof(buf));

	/* 关闭设备文件 */
	close(fd);

	return 0;
}
