FcgiServer: FcgiServer.cpp MysqlConnector.h
	g++ FcgiServer.cpp MysqlConnector.cpp -lfcgi++ -lfcgi -lcgicc -lrudecgi $(shell mysql_config --cflags --libs) -o EntryTask