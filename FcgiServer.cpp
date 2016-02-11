//g++ FcgiServer.cpp -lfcgi++ -lfcgi -lcgicc -o EntryTask
//spawn-fcgi -p 8000 -n EntryTask
#include <iostream>
#include <sstream>
#include <fstream>
#include "fcgio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iterator>
#include <algorithm>

#include <cgicc/CgiDefs.h>
#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTMLClasses.h>

#include "TcpClient.h"

using namespace std;
using namespace cgicc;

const unsigned int STDIN_MAX = 100000000;
TcpClient tc;


string get_upload_request_content(const FCGX_Request & request);
string get_request_cookie(const FCGX_Request &request);
string get_request_content(const FCGX_Request &request);
vector<string> split(const string &str, const string& delim, const bool keep_empty);
string get_post_parameter_value(const string &str);


void process_request(const char *request_uri, const FCGX_Request &request);
string check_sign_in_status(const FCGX_Request &request);

void welcome_page(bool hasErrorMessage);
void user_login(const FCGX_Request &request);
void show_profile(const FCGX_Request &request);
void change_profile(const FCGX_Request &request);
void update_nickname(const FCGX_Request &request);
void update_picture(const FCGX_Request &request);
void path_not_found(const FCGX_Request &request);

int main(void)
{
    // Backup the stdio streambufs
    streambuf * cin_streambuf  = cin.rdbuf();
    streambuf * cout_streambuf = cout.rdbuf();
    streambuf * cerr_streambuf = cerr.rdbuf();

    FCGX_Request request;
    tc.conn("localhost", 8080);

    FCGX_Init();
    FCGX_InitRequest(&request, 0, 0);

    while (FCGX_Accept_r(&request) == 0)
    {
        Cgicc formData;
        fcgi_streambuf cin_fcgi_streambuf(request.in);
        fcgi_streambuf cout_fcgi_streambuf(request.out);
        fcgi_streambuf cerr_fcgi_streambuf(request.err);

        cin.rdbuf(&cin_fcgi_streambuf);
        cout.rdbuf(&cout_fcgi_streambuf);
        cerr.rdbuf(&cerr_fcgi_streambuf);
       
        const char* request_uri = FCGX_GetParam("REQUEST_URI", request.envp);
        process_request(request_uri, request);

    }

    // restore stdio streambufs
    cin.rdbuf(cin_streambuf);
    cout.rdbuf(cout_streambuf);
    cerr.rdbuf(cerr_streambuf);

    return 0;
}

void process_request(const char *request_uri, const FCGX_Request &request) 
{
    if (strcmp(request_uri, "/welcome") == 0)
    {
        welcome_page(false);
    }
    else if (strcmp(request_uri, "/login") == 0)
    {
        user_login(request);
    }
    else if (strcmp(request_uri, "/show_profile") == 0)
    {
        show_profile(request);
    }
    else if (strcmp(request_uri, "/change_profile") == 0)
    {
        change_profile(request);
    }
    else if (strcmp(request_uri, "/update_nickname") == 0)
    {
        update_nickname(request);
    }
    else if (strcmp(request_uri, "/update_picture") == 0)
    {
        update_picture(request);
    }
    else
    {
        path_not_found(request);
    }
}

string check_sign_in_status(const FCGX_Request &request)
{
    string cookie = get_request_cookie(request);
    if (cookie == "")
    {
        return "";
    }

    vector<string> token = split(cookie, "=", true);
    string username = token[1];

    return username;
}

void welcome_page(bool hasErrorMessage)
{
    cout << "Content-type: text/html\r\n";
    cout << "\r\n";
    cout << "<html>\n";
    cout << "  <head>\n";
    cout << "    <title>Welcome Page</title>\n";
    cout << "  </head>\n";
    cout << "  <body>\n";
    if (hasErrorMessage)
    {
        cout << "    <h1>Wrong password or non-existing user!</h1>\n";
    }
    else
    {
        cout << "    <h1>Welcome!</h1>\n";
    }
    cout << "    <form action=\"login\" method=\"post\">";
    cout << "      Username: <input type=\"text\" name=\"username\"><br>";
    cout << "      Password: <input type=\"text\" name=\"password\"><br>";
    cout << "      <button type=\"submit\">Login</button><br>";
    cout << "    </form>";
    cout << "  </body>\n";
    cout << "</html>\n";

}
void user_login(const FCGX_Request &request)
{
    string content = get_request_content(request);
    vector<string> contentList = split(content, "&", true);

    if (contentList.size() != 2) {
        //Error
    }
        
    string username = get_post_parameter_value(contentList[0]);
    string password = get_post_parameter_value(contentList[1]);

    if (tc.match_username_password(username, password))
    {
        cout << "Content-type: text/html\r\n";
        cout << "Set-Cookie: username=" << username << "\n\n";
        cout << "<html>\n";
        cout << "  <head>\n";
        cout << "    <title>Successfully login</title>\n";
        cout << "  </head>\n";
        cout << "  <body>\n";
        cout << "You can direct to profile page by clicking <a href=\"./show_profile\">profile page</a>\n";
        cout << "  </body>\n";
        cout << "</html>\n";
    }
    else
    {
        welcome_page(true);
    }
}
void show_profile(const FCGX_Request &request)
{
    string username = check_sign_in_status(request);
    if (username == "")
    {
        welcome_page(false);
        return;
    }
    vector<string> user_info = tc.retrieve_user_info(username);
    string nickname = user_info[0];
    string profile_link = user_info[1];

    cout << "Content-type: text/html\r\n";
    cout << "\r\n";
    cout << "<html>\n";
    cout << "  <head>\n";
    cout << "    <title>Profile Page</title>\n";
    cout << "  </head>\n";
    cout << "  <body>\n";
    cout << "    <h1>Profile</h1>\n";
    cout << "    <h4>Username: " << username << "</h4>" << endl;
    cout << "    <h4>Nickname: " << nickname << "</h4>" << endl;
    cout << "    <img src=\"" << profile_link << "\" height=\"200\">";

    cout << "    <form action=\"change_profile\" method=\"post\">";
    cout << "      <button type=\"submit\">Change Profile</button><br>";
    cout << "    </form>";

    cout << "  </body>\n";
    cout << "</html>\n";
}
void change_profile(const FCGX_Request &request)
{
    string username = check_sign_in_status(request);
    if (username == "")
    {
        welcome_page(false);
        return;
    }

    cout << "Content-type: text/html\r\n";
    cout << "\r\n";
    cout << "<html>\n";
    cout << "  <head>\n";
    cout << "    <title>Change Profile</title>\n";
    cout << "  </head>\n";
    cout << "  <body>\n";
    cout << "    <h1>Change Profile</h1>\n";

    cout << "    <form action=\"update_nickname\" method=\"post\">";
    cout << "      <input type=\"text\" name=\"nickname\"></br>\n";
    cout << "      <button type=\"submit\">Change Nickname</button><br>";
    cout << "    </form>";


    cout << "    <form action=\"update_picture\" enctype=\"multipart/form-data\" method=\"post\">";
    cout << "      <input type=\"file\" name=\"image\" accept=\"image/png\"></br>\n";
    cout << "      <button type=\"submit\">Change Picture</button><br>";
    cout << "    </form>";

    cout << "  </body>\n";
    cout << "</html>\n";
}
void update_nickname(const FCGX_Request &request)
{
    string username = check_sign_in_status(request);
    if (username == "")
    {
        welcome_page(false);
        return;
    }
    cout << "Content-type: text/html\r\n";
    cout << "\r\n";
    cout << "<html>\n";
    cout << "  <head>\n";
    cout << "    <title>Update Nickname</title>\n";
    cout << "  </head>\n";
    cout << "  <body>\n";
    cout << "    <h1>Update Nickname</h1>\n";

    string content = get_request_content(request);
    vector<string> contentList = split(content, "&", true);

    if (contentList.size() != 1) {
        // content error
        cout << "    <h1>Error in post content.</h1>\n";
    }
        
    string new_nickname = get_post_parameter_value(contentList[0]);
    if (tc.update_nickname(username, new_nickname))
    {
        cout << "    <h3>Update Successfully!</h3>\n";
        cout << "You can direct to profile page by clicking <a href=\"./show_profile\">profile page</a>";
    }
    else
    {
        // update failed.
        cout << "    <h3>Update Failed!</h3>\n";
        cout << "You can direct to profile page by clicking <a href=\"./show_profile\">profile page</a>";
    }

    cout << "  </body>\n";
    cout << "</html>\n";
}
void update_picture(const FCGX_Request &request)
{
    string username = check_sign_in_status(request);
    if (username == "")
    {
        welcome_page(false);
        return;
    }
    string params = get_upload_request_content(request);
    stringstream ss(params);

    cout << "Content-type:text/html\n\n";
    cout << "<html>\n";
    cout << "<head>\n";
    cout << "<title>Update Picture</title>\n";
    cout << "</head>\n";
    cout << "<body>\n";
    string line;
    vector<string> lines;
    while (getline(ss, line))
    {
        //cout << line << "\n" << endl;
        lines.push_back(line);
    }
    lines.erase(lines.begin() + 0, lines.begin() + 4);
    lines.erase(lines.begin() + (lines.size() - 1));
    lines.erase(lines.begin() + (lines.size() - 1));
    string filename = "images/" + username + ".png";
    ofstream oFile;
    oFile.open(filename.c_str(), ios::binary);
    for (int i = 0; i < lines.size(); i++)
    {
        //cout << lines[i] << "<br>\n";
        oFile << lines[i] << "\n";
    }
    oFile.close();
    
    cout << "    <h3>Update Successfully!</h3>\n";
    cout << "You can direct to profile page by clicking <a href=\"./show_profile\">profile page</a>";
    cout << "</body>\n";
    cout << "<html>\n";

}
void path_not_found(const FCGX_Request &request)
{
    welcome_page(false);
}

vector<string> split(const string &str, const string& delim, const bool keep_empty) {
    vector<string> result;

    if (delim.empty()) {
        result.push_back(str);
        return result;
    }
    string::const_iterator substart = str.begin(), subend;

    while (true) {
        subend = search(substart, str.end(), delim.begin(), delim.end());
        string temp(substart, subend);

        if (keep_empty || !temp.empty()) {
            result.push_back(temp);
        }

        if (subend == str.end()) {
            break;
        }
        substart = subend + delim.size();
    }

    return result;
}

string get_post_parameter_value(const string &str)
{
    vector<string> tempList = split(str, "=", true);
    return tempList[1];
}

string get_request_cookie(const FCGX_Request &request)
{
    char *cookie = FCGX_GetParam("HTTP_COOKIE", request.envp);

    if (cookie)
    {
        return string(cookie);
    }

    return "";
}

string get_request_content(const FCGX_Request &request)
{
    char* contentLengthStr = FCGX_GetParam("CONTENT_LENGTH", request.envp);
    unsigned long contentLength = STDIN_MAX;

    if (contentLengthStr) {
        contentLength = strtol(contentLengthStr, &contentLengthStr, 10);

        if (*contentLengthStr) {//Longer that STDIN_MAX
            cerr << "Can't Parse 'CONTENT_LENGTH = '"
                 << FCGX_GetParam("CONTENT_LENGTH", request.envp)
                 << "'. Consuming stdin up to " << STDIN_MAX << endl;
        }

        if (contentLength > STDIN_MAX) {
            contentLength = STDIN_MAX;
        }
    } else {
        //Content length is missing
        contentLength = 0;
    }

    char* contentBuffer = new char[contentLength];
    cin.read(contentBuffer, contentLength);
    contentLength = cin.gcount();

    do cin.ignore(1024); while (cin.gcount() == 1024);

    string content(contentBuffer, contentLength);
    delete [] contentBuffer;
    return content;
}

string get_upload_request_content(const FCGX_Request & request) {
    char * content_length_str = FCGX_GetParam("CONTENT_LENGTH", request.envp);
    unsigned long content_length = STDIN_MAX;

    if (content_length_str)
    {
        content_length = strtol(content_length_str, &content_length_str, 10);
        if (*content_length_str)
        {
            cerr << "Can't Parse 'CONTENT_LENGTH='"
                 << FCGX_GetParam("CONTENT_LENGTH", request.envp)
                 << "'. Consuming stdin up to " << STDIN_MAX << endl;
        }
        if (content_length > STDIN_MAX)
        {
            content_length = STDIN_MAX;
        }
    }
    else
    {
        // Do not read from stdin if CONTENT_LENGTH is missing
        content_length = 0;
    }

    char * content_buffer = new char[content_length];
    cin.read(content_buffer, content_length);
    content_length = cin.gcount();

    // Chew up any remaining stdin - this shouldn't be necessary
    // but is because mod_fastcgi doesn't handle it correctly.

    // ignore() doesn't set the eof bit in some versions of glibc++
    // so use gcount() instead of eof()...
    do cin.ignore(1024); while (cin.gcount() == 1024);

    string content(content_buffer, content_length);
    delete [] content_buffer;
    return content;
}