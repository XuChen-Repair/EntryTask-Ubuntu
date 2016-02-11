#include "TcpClient.h"
#include <sstream>
 
using namespace std;
 
TcpClient::TcpClient()
{
    sock = -1;
    port = 0;
    address = "";
}

bool TcpClient::match_username_password(string username, string password)
{
    string message = "matching&username=" + username + "&password=" + password+"\n";
    send_data(message);
    string reply = receive(1024);

    if (reply.compare("true") == 0) {
        return true;
    }

    return false;
}

vector<string> TcpClient::retrieve_user_info(string username)
{
    string message = "retrieve&username=" + username + "\n";
    send_data(message);
    string reply = receive(1024);

    vector<string> temp = split(reply.c_str(), '&');
    vector<string> result;
    result.push_back(temp[1]);
    result.push_back(temp[2]);

    return result;
}

bool TcpClient::update_profile_link(string username, string profile_link)
{
    string message = "update_profile_link&username=" + username + "&profile_link=" + profile_link + "\n";
    send_data(message);
    string reply = receive(1024);

    if (reply.compare("true") == 0) {
        return true;
    }

    return false;
}

bool TcpClient::update_nickname(string username, string nickname)
{
    string message = "update_nickname&username=" + username + "&nickname=" + nickname + "\n";
    send_data(message);
    string reply = receive(1024);

    if (reply.compare("true") == 0) {
        return true;
    }

    return false;
}

 
/**
    Connect to a host on a certain port number
*/
bool TcpClient::conn(string address , int port)
{
    //create socket if it is not already created
    if(sock == -1)
    {
        //Create socket
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)
        {
            perror("Could not create socket");
        }
         
        //cout<<"Socket created\n";
    }
    else    {   /* OK , nothing */  }
     
    //setup address structure
    if(inet_addr(address.c_str()) == -1)
    {
        struct hostent *he;
        struct in_addr **addr_list;
         
        //resolve the hostname, its not an ip address
        if ( (he = gethostbyname( address.c_str() ) ) == NULL)
        {
            //gethostbyname failed
            herror("gethostbyname");
            cout<<"Failed to resolve hostname\n";
             
            return false;
        }
         
        //Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
        addr_list = (struct in_addr **) he->h_addr_list;
 
        for(int i = 0; addr_list[i] != NULL; i++)
        {
            //strcpy(ip , inet_ntoa(*addr_list[i]) );
            server.sin_addr = *addr_list[i];
             
            cout<<address<<" resolved to "<<inet_ntoa(*addr_list[i])<<endl;
             
            break;
        }
    }
     
    //plain ip address
    else
    {
        server.sin_addr.s_addr = inet_addr( address.c_str() );
    }
     
    server.sin_family = AF_INET;
    server.sin_port = htons( port );
     
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    //cout<<"Connected\n";
    return true;
}
 
/**
    Send data to the connected host
*/
bool TcpClient::send_data(string data)
{
    //Send some data
    if( send(sock , data.c_str() , strlen( data.c_str() ) , 0) < 0)
    {
        perror("Send failed : ");
        return false;
    }
    //cout<<"Data send\n";
     
    return true;
}
 
/**
    Receive data from the connected host
*/
string TcpClient::receive(int size=512)
{
    char buffer[size];
    string reply;
     
    //Receive a reply from the server
    int count = recv(sock , buffer , sizeof(buffer) , 0);
    if( count < 0)
    {
        puts("recv failed");
    }
    
    //cout << "count: " << count << endl;
    reply = char_to_str(buffer, count);
    return reply;
}

vector<string> TcpClient::split(string str, char delimiter) {
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;
  
  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }
  
  return internal;
}

string TcpClient::char_to_str(char *buf, int length) {
  char temp[length + 1];

  int i = 0;
  while(length != i) {
    temp[i] = *buf;
    
    *buf++;
    i++;
  }
  temp[i] = '\0';

  return temp;
}