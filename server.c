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
#include <mysql/mysql.h>
#include "/usr/include/mariadb/mysql.h"

#define PORT 8080
#define MAXSZ 100

#define DB_HOST "smartpark-webservice.c5jmgn2iozvr.ap-northeast-2.rds.amazonaws.com"
#define DB_USER "admin"
#define DB_PASS "ht5893083"
#define DB_NAME "smartpark"
#define CHOP(x) x[strlen(x) - 1] = ' '

#define BUF_LEN 128			//buffer length
#define SERV_IP_ADDR "192.168.4.2"		//our IP addr
#define SERV_PORT 9000		//registered port 영역, server socket로 사용하는 영역의 것으로 define
int main(int argc,char *argv[])
{    
	// char buffer[BUF_LEN];    
	// struct sockaddr_in server_addr;		//구조체 선언
	// struct sockaddr_in client_addr;		// struct 태그명 변수명
	// char temp[20];    					//
	// int server_fd,client_fd;			//file descripter(파일 지정번호)
	// int len,msg_size;					//msg_size?ok	//what is the mean of len?
	// char test[20];						//test...?

	MYSQL       *connection = NULL;
	MYSQL		conn;
	MYSQL_RES   *sql_result;
	MYSQL_ROW   sql_row;
	int       query_stat;

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

	query_stat = mysql_query(connection, "select * from con0001");
	if (query_stat != 0)
	{
		fprintf(stderr, "2Mysql query error : %s", mysql_error(&conn));
		return 1;
	}

    printf("Connection Sucess!");

	// sql_result = mysql_store_result(connection);
	//  mysql_free_result(sql_result);



	mysql_close(&conn);
	return 0;
}