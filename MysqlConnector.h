//g++ -o output $(mysql_config --cflags) main.cpp MysqlConnector.cpp $(mysql_config --libs)
#ifndef MysqlConnector_h
#define MysqlConnector_h

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <mysql/mysql.h>

using namespace std;

class MysqlConnector {

public:
    MysqlConnector() {
        host = (char *)"localhost";
    	user = (char *)"root";
    	password = (char *)"7360300xc";
    	dbname = (char *)"EntryTask";
    
    	port = 3306;
    	unix_socket = NULL;
    	flag = 0;
    }

    bool isPasswordMatched(string username, string password);
    bool hasUser(string username);
    vector<string> retrieveUserInfo(string username);
    bool updateProfileLink(string username, string profile_link);
    bool updateNickname(string username, string nickname);

    /* For Setup Use */
    bool dropTable();
    bool createTable();
    bool insertNewUser(string username, string nickname, string profile_link, string password);

    void init();
    void close();
    
private:
	char *host;// = (char *)"localhost";
    char *user;// = (char *)"root";
    char *password;// = (char *)"7360300xc";
    char *dbname;// = (char *)"EntryTask";
    
    unsigned int port;// = 3306;
    char* unix_socket;// = NULL;
    unsigned int flag;// = 0;
    
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    
    //void initializeMysqlConnector();
};

#endif /* MysqlConnector_h */