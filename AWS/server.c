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
#include "/home/ec2-user/mysql-connector/include/mysql.h"

#define PORT 8080
#define MAXSZ 100

#define DB_HOST "smartpark-webservice.c5jmgn2iozvr.ap-northeast-2.rds.amazonaws.com"	// AWS RDS end point
#define DB_USER "admin"			// DB user name
#define DB_PASS "ht5893083"		// DB password
#define DB_NAME "smartpark"		// DB name using now
#define CHOP(x) x[strlen(x) - 1] = ' '

#define BUF_LEN 128			//buffer length
#define SERV_IP_ADDR "54.180.52.108"		//our IP addr
#define SERV_PORT 9000		//registered port 영역, server socket로 사용하는 영역의 것으로 define
int main(int argc,char *argv[])
{    
	char buffer[BUF_LEN];    
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	char temp[20];
	int server_fd,client_fd;
	int len,msg_size;
	char test[20];

	MYSQL       *connection = NULL;
	MYSQL		conn;
	MYSQL_RES   *sql_result;
	MYSQL_ROW   sql_row;
	int       query_stat;

	/* DB Connection */
	mysql_init(&conn);
	connection = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char *)NULL, 0);


	if (connection == NULL)
	{
		printf("%d 에러 : %s, %d\n", mysql_errno(&conn), mysql_error(&conn));
		return 1;
	}

	if (mysql_select_db(&conn, DB_NAME))
	{
		printf("%d 에러 : %s, %d\n", mysql_errno(&conn), mysql_error(&conn));
		return 1;
	}

	query_stat = mysql_query(connection, "select * from parking");
	if (query_stat != 0)
	{
		fprintf(stderr, "2Mysql query error : %s", mysql_error(&conn));
		return 1;
	}

	sql_result = mysql_store_result(connection);
	 mysql_free_result(sql_result);



	/* Socket */
	if((server_fd = socket(PF_INET,SOCK_STREAM,0)) == -1) 
	// server socket생성/파일 지정번호 반환, TCP/IP
	{        
		printf("Server: can not Open Socket\n");        // socket 생성 error
		exit(0);    
	}       
	memset(&server_addr,0x00,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(SERV_PORT);       
	printf("1\n");
	if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0 )    // port number 설정
	{        printf("Server:*** cat not bind local addrss\n");        
			 exit(0);            
	}
	printf("2\n");	
	if(listen(server_fd,5) < 0)    		// client의 접속 queue크기 설정
	{        
		printf("Server:** cat not listen connnect.\n");        
		exit(0);    
	}
	printf("3\n");	
	memset(buffer,0x00,sizeof(buffer));
	len = sizeof(client_addr);
	printf("Server : wating connection request.\n");         


	while(1)    
	{        
		//parent process waiting to accept a new connection
		printf("\n*****server waiting for new client connection:*****\n");
		len = sizeof(client_addr);
		client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &len);
		printf("connected to client: %s\n", inet_ntoa(client_addr.sin_addr));

		pid_t pid;
		//child process is created for serving each new clients
		pid = fork();
		if (pid == 0)//child process rec and send
		{
			//receive from client
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
				char sArr[20][20] = { 0 };

				// parsing
				char *token = NULL;
				char str2[] = "|";	// delimeter

				token = strtok(mes, str2);
				int i = 0;
				while (token != NULL)
				{
					strcpy(sArr[i], token);
					token = strtok(NULL, str2);
					i++;
				}

                // set time now
                time_t t = time(NULL);
                struct tm tm = *localtime(&t);
                char now[20];
                sprintf(now, "%d-%d-%d %d:%d:%d", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

                // get state from 'parking' table
                char query[1001];
                sprintf(query, "SELECT state FROM parking");

                query_stat = mysql_query(connection, query);
                if (query_stat != 0)
                {
                    fprintf(stderr, "3Mysql query error : %s\n", mysql_error(&conn));
                }
                sql_result = mysql_store_result(connection);

				// execute UPDATE query
                int j = 0;
                while ((sql_row = mysql_fetch_row(sql_result)) != NULL){
                    printf("parked[%d] now : %s\t", j, sql_row[0]);
                    if (sql_row[0][0] != sArr[2*j+1][0]){     // if state changed
                        printf("changed : %s\n", sql_row[0], sArr[2*j+1]);
                        char query[1001];
                        sprintf(query, "UPDATE parking SET state=%s,park_time='%s' WHERE id=%s", sArr[2*j+1], now, sArr[2*j]);
                        mysql_query(&conn, query);

                        query_stat = mysql_query(connection, query);
                        if (query_stat != 0)
                        {
                            fprintf(stderr, "3Mysql query error : %s\n", mysql_error(&conn));
                        }

                    }
                    j++;
                }
                printf("\n-----waiting next state-----");

			}//close interior while
			exit(0);
		}
		else
		{
			close(client_fd);//sock is closed BY PARENT
		}
	}        		
	mysql_close(&conn);
	return 0;
}