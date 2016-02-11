#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "MysqlConnector.h"

using namespace std;

int main() {

	MysqlConnector mc = *new MysqlConnector();

	mc.init();
	if (mc.isExistingUser("test_username", "test_password")) {
		cout << "Existing!" << endl;
	} else {
		cout << "Failed!" << endl;
	}

	mc.close();
	return 0;
}
