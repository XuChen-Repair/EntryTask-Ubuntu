# EntryTask-Ubuntu
C++ based FastCGI Server + TCP Backend using epoll

##Setup
1.  Install Vagrant [here](https://www.vagrantup.com/downloads.html).
2.  Clone the repo.
3.  Navigate to the project folder and run `vagrant up`.

##Running Server
1.  Run `vagrant ssh`. After enter into vitual machine, run `make`, which would compile and build all necessary files.
2.  Run `./server 8080`.
3.  Run `spawn-fcgi -p 8000 -F 20 -n EntryTask` in a new Terminal window. You could see the status notification of TCP server connection. The port number can be changed, but you have to change the setting in nginx configuration as well.

##Design
Below shows the design diagram of this project:

![alt tag](https://github.com/XuChen-Repair/EntryTask-Ubuntu/blob/master/EntryTaskDesign.png)

Nginx works as a server to receive clients' requests, which will be handled by FastCGI Server. If the process need to handle login authentication or data retrival, then FastCGI Server will connect to TCP Server, which is based on epoll and communicate with database using Mysql C API.

##Benchmarking
Requirement1: Support up to 1000 login requests per second (at least 200 unique users).
Results are shown below (10 processes with 200 unique users).



| No. of requests| Time          |
| -------------  | ------------- |
| 10 * 1200      | 8s            |
| 10 * 2000      | 12s           |
| 10 * 4000      | 21s           |
| 10 * 8000      | 41s           |


Requirement2: Support up to 1000 concurrent http requests.

