import http.client
import urllib
import sys

def login(username, pswd):
	params = urllib.parse.urlencode({'username': username, 'pswd': pswd})
	headers = {"Content-type": "application/x-www-form-urlencoded", "Accept": "text/plain"}
	conn = http.client.HTTPConnection("localhost")
	conn.request("POST", "/login", params, headers)
	r = conn.getresponse()
	if r.status != 200:
		print("login error")

if __name__ == '__main__':
	for x in range(1, 400):
		login("test{}".format(x % 200), "test_password")