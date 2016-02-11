//gcc -o output $(mysql_config --cflags) connect.c $(mysql_config --libs)

#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>

static char *host = "localhost";
static char *user = "root";
static char *password = "7360300xc";
static char *dbname = "EntryTask";

static unsigned int port = 3306;
static char *unix_socket = NULL;
static unsigned int flag = 0;

int main() 
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;

	conn = mysql_init(NULL);

	if(!(mysql_real_connect(conn, host, user, password, dbname, port, unix_socket, flag))) 
	{
		fprintf(stderr, "Error: %s [%d]\n", mysql_error(conn), mysql_errno(conn));
		exit(1);
	}

	printf("Connection Successful!\n\n");

	mysql_query(conn, "SELECT * FROM User;");
	res = mysql_store_result(conn);

	//row = mysql_fetch_row(res);
	while((row = mysql_fetch_row(res)) != NULL)
	{
		printf("%s\t%s\t%s\n", row[0], row[1], row[2]);
		//row = mysql_fetch_row(res);
	}


	mysql_free_result(res);
	mysql_close(conn);
	return 0;
}