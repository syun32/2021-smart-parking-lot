/*
sctest
수신 데이터 그대로 터미널에 출력
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAXSZ 100

#define BUF_LEN 128					 // buffer length
#define SERV_IP_ADDR "54.180.52.108" // our IP addr
#define SERV_PORT 9000				 // registered port
int main(int argc, char *argv[])
{
	char buffer[BUF_LEN];
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	char temp[20];
	int server_fd, client_fd;
	int len, msg_size;
	char test[20];

	if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	// server socket생성/파일 지정번호 반환, TCP/IP
	{
		printf("Server: can not Open Socket\n"); // socket 생성 error
		exit(0);
	}
	memset(&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(SERV_PORT);
	printf("1\n");
	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) // port number 설정
	{
		printf("Server:*** cat not bind local addrss\n");
		exit(0);
	}
	printf("2\n");
	if (listen(server_fd, 5) < 0) // client의 접속 queue크기 설정
	{
		printf("Server:** cat not listen connnect.\n");
		exit(0);
	}
	printf("3\n");
	memset(buffer, 0x00, sizeof(buffer)); // buffer 초기화
	len = sizeof(client_addr);
	printf("Server : wating connection request.\n");

	while (1)
	{
		// parent process waiting to accept a new connection
		printf("\n*****server waiting for new client connection:*****\n");
		len = sizeof(client_addr);
		client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &len);
		printf("connected to client: %s\n", inet_ntoa(client_addr.sin_addr));

		pid_t pid;
		// child process is created for serving each new clients
		pid = fork();
		if (pid == 0) // child process rec and send
		{
			// receive from client
			while (1)
			{
				msg_size = recv(client_fd, buffer, BUF_LEN, 0);
				if (msg_size == 0)
				{
					close(client_fd);
					break;
				}
				buffer[msg_size] = 0;

				char mes[BUF_LEN];
				strcpy(mes, buffer);

				printf(mes); // mes test 출력

			} // close interior while
			exit(0);
		}
		else
		{
			close(client_fd); // sock is closed BY PARENT
		}
	}
	return 0;
}