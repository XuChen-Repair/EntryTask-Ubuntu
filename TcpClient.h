#ifndef TcpClient_h
#define TcpClient_h

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>

using namespace std;

class TcpClient
{
private:
    int sock;
    string address;
    int port;
    struct sockaddr_in server;

    bool send_data(string data);
    string receive(int);
    string char_to_str(char *buf, int length);
    vector<string> split(string str, char delimiter);
     
public:
    TcpClient();
    bool conn(string, int);
    bool match_username_password(string, string);
    vector<string> retrieve_user_info(string);
    bool update_profile_link(string, string);
    bool update_nickname(string, string);

};

#endif /* TcpClient_h */