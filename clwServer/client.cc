#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
//#include "wrap.h"

#define SERV_IP "192.168.48.139"  //"127.0.0.1"
#define SERV_PORT 6666

int main()
{
	int cfd; //套接字描述符
	struct sockaddr_in serv_addr;
	char buf[BUFSIZ];
	int n;

	cfd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, SERV_IP, &serv_addr.sin_addr.s_addr); //将十进制串转换为网络字节序
									 //serv_addr.sin_addr.s_addr=htonl(SERV_IP);

	connect(cfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	while (1)
	{
		fgets(buf, sizeof(buf), stdin);  //从文件流读取一行（包括换行符\n）送到缓冲区buf中
		write(cfd, buf, strlen(buf));    //从buf中向cfd所代表的文件中写入除换行符之外的所有字节
		n = read(cfd, buf, sizeof(buf)); //从cfd所代表的文件中请求数据读入到buf中
		write(STDOUT_FILENO, buf, n);    //从buf中读取n个字节显示到屏幕上
	}
	close(cfd);

	return 0;
}
