#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sstream>
#include <iostream>

#include "MysqlConnector.h"

using namespace std;

int main() 
{
	MysqlConnector mc;
  	mc.init();

  	if (mc.dropTable() == false)
  	{
  		cout << "Error in droping table.\n" << endl;
  	}

  	if (mc.createTable() == false)
  	{
  		cout << "Error in creating table.\n" << endl;
  	}


	//string username;
	string nickname = "test_nickname";
	string password = "test_password";
	
  	for (unsigned int i = 1; i <= 10000000; i++)
  	{
  		//cout << i << "\n" << endl;
  		string username = "test";
  		username.append(to_string(i));
  		string profile_link = username;
  		profile_link.append(".png");
  		//cout << "username: " << username << "\n" << endl;
  		//cout << "profile_link: " << profile_link << "\n" << endl;
  		if (mc.insertNewUser(username, nickname, profile_link, password) == false)
  		{
  			cout << "Error in inserting user " + username + ".\n" << endl;
  		}
  	}	
  	//}

  	cout << "Set up finished!\n" << endl;
  	mc.close();

  	return 0;
}