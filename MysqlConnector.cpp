#include "MysqlConnector.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
using namespace std;

bool MysqlConnector::dropTable() {
  string query_string = "DROP TABLE User;";

  if (mysql_real_query(conn, query_string.c_str(), query_string.size()) == 0) {
    return true;
  }

  return false;
}

bool MysqlConnector::createTable() {
  string query_string = "CREATE TABLE User(username varchar(30), nickname varchar(20), profile_link varchar(30), password varchar(20));";

  if (mysql_real_query(conn, query_string.c_str(), query_string.size()) == 0) {
    return true;
  }

  return false;
}

bool MysqlConnector::insertNewUser(string username, string nickname, string profile_link, string password) {
  string query_string = "INSERT INTO User (username, nickname, profile_link, password) values(\'" + username + 
                        "\', \'" + nickname + "\', \'" + profile_link + "\', \'" + password + "\');";

  //cout << query_string << endl;
  if (mysql_real_query(conn, query_string.c_str(), query_string.size()) == 0) {
    return true;
  }

  return false;
}

bool MysqlConnector::isPasswordMatched(string username, string password) {
  bool result;

  string query_string = "SELECT password FROM User WHERE username = '" + username + "';";

  mysql_query(conn, query_string.c_str());
  res = mysql_store_result(conn);
    
  if ((row = mysql_fetch_row(res)) != NULL) {
    if (strcmp(row[0], password.c_str()) == 0) {
      result = true;
    } else {
      result = false;
    }

  } else {
    result = false;
  }
    
  mysql_free_result(res);
    
  return result;
}

vector<string> MysqlConnector::retrieveUserInfo(string username) {
  vector<string> result;

  string query_string = "SELECT nickname, profile_link FROM User WHERE username = '" + username + "';";

  mysql_query(conn, query_string.c_str());
  res = mysql_store_result(conn);
    
  if ((row = mysql_fetch_row(res)) != NULL) {
    result.push_back(row[0]);
    result.push_back(row[1]);
  }

  return result;
}

bool MysqlConnector::updateProfileLink(string username, string profile_link) {
  string query_string = "UPDATE User SET profile_link = '" + profile_link + "' WHERE username = '" + username + "';";
    
  if (mysql_real_query(conn, query_string.c_str(), query_string.size()) == 0) {
    return true;
  }

  return false;
}

bool MysqlConnector::updateNickname(string username, string nickname) {
  string query_string = "UPDATE User SET nickname = '" + nickname + "' WHERE username = '" + username + "';";
    
  if (mysql_real_query(conn, query_string.c_str(), query_string.size()) == 0) {
    return true;
  }

  return false;
}


bool MysqlConnector::hasUser(string username) {
  bool result;

  string query_string = "SELECT * FROM User WHERE username = '" + username + "';";

  mysql_query(conn, query_string.c_str());
  res = mysql_store_result(conn);
    
  if ((row = mysql_fetch_row(res)) != NULL) {
    result = true;
  } else {
    result = false;
  }

  return result;
}

void MysqlConnector::init() {
   conn = mysql_init(NULL);
   
   if(!(mysql_real_connect(this->conn, this->host, this->user, this->password, this->dbname, this->port, this->unix_socket, this->flag))) {
       fprintf(stderr, "Error: %s [%d]\n", mysql_error(conn), mysql_errno(conn));
       exit(1);
   }
    
    printf("Connection Successful!\n\n");
}

void MysqlConnector::close() {
    mysql_close(conn);
}