#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <vector>
#include <sstream>
#include <iostream>

#include "MysqlConnector.h"

#define MAXEVENTS 64

using namespace std;

string char_to_str(char *buf, int length)
{
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

vector<string> split(string str, char delimiter)
{
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;
  
  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }
  
  return internal;
}

static int make_socket_non_blocking (int sfd)
{
  int flags, s;

  flags = fcntl (sfd, F_GETFL, 0);
  if (flags == -1)
  {
    perror ("fcntl");
    return -1;
  }

  flags |= O_NONBLOCK;
  s = fcntl (sfd, F_SETFL, flags);
  if (s == -1)
  {
    perror ("fcntl");
    return -1;
  }

  return 0;
}

static int create_and_bind (char *port)
{
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  int s, sfd;

  memset (&hints, 0, sizeof (struct addrinfo));
  hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
  hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
  hints.ai_flags = AI_PASSIVE;     /* All interfaces */

  s = getaddrinfo (NULL, port, &hints, &result);
  if (s != 0)
  {
    fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
    return -1;
  }

  for (rp = result; rp != NULL; rp = rp->ai_next)
  {
    sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (sfd == -1)
      continue;

    s = bind (sfd, rp->ai_addr, rp->ai_addrlen);
    if (s == 0)
    {
      /* We managed to bind successfully! */
      break;
    }

    close (sfd);
  }

  if (rp == NULL)
  {
    fprintf (stderr, "Could not bind\n");
    return -1;
  }

  freeaddrinfo (result);

  return sfd;
}

int main (int argc, char *argv[])
{
  int sfd, s;
  int efd;
  struct epoll_event event;
  struct epoll_event *events;
  MysqlConnector mc;
  mc.init();

  if (argc != 2)
  {
    fprintf (stderr, "Usage: %s [port]\n", argv[0]);
    exit (EXIT_FAILURE);
  }

  sfd = create_and_bind (argv[1]);
  if (sfd == -1)
    abort ();

  s = make_socket_non_blocking (sfd);
  if (s == -1)
    abort ();

  s = listen (sfd, SOMAXCONN);
  if (s == -1)
  {
    perror ("listen");
    abort ();
  }

  efd = epoll_create1 (0);
  if (efd == -1)
  {
    perror ("epoll_create");
    abort ();
  }

  event.data.fd = sfd;
  event.events = EPOLLIN | EPOLLET;
  s = epoll_ctl (efd, EPOLL_CTL_ADD, sfd, &event);
  if (s == -1)
  {
    perror ("epoll_ctl");
    abort ();
  }

  /* Buffer where events are returned */
  events = (epoll_event *)calloc (MAXEVENTS, sizeof(event));

  /* The event loop */
  while (1)
  {
    int n, i;

    n = epoll_wait (efd, events, MAXEVENTS, -1);
    for (i = 0; i < n; i++)
    {
      if ((events[i].events & EPOLLERR) ||
          (events[i].events & EPOLLHUP) ||
          (!(events[i].events & EPOLLIN)))
      {
              /* An error has occured on this fd, or the socket is not
                 ready for reading (why were we notified then?) */
        fprintf (stderr, "epoll error\n");
        close (events[i].data.fd);
        continue;
      }

      else if (sfd == events[i].data.fd)
      {
              /* We have a notification on the listening socket, which
                 means one or more incoming connections. */
        while (1)
        {
          struct sockaddr in_addr;
          socklen_t in_len;
          int infd;
          char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

          in_len = sizeof in_addr;
          infd = accept (sfd, &in_addr, &in_len);
          if (infd == -1)
          {
            if ((errno == EAGAIN) ||
                (errno == EWOULDBLOCK))
            {
              /* We have processed all incoming connections. */
              break;
            }
            else
            {
              perror ("accept");
              break;
            }
          }

          s = getnameinfo (&in_addr, in_len,
                            hbuf, sizeof hbuf,
                            sbuf, sizeof sbuf,
                            NI_NUMERICHOST | NI_NUMERICSERV);
          if (s == 0)
          {
            printf("Accepted connection on descriptor %d "
                   "(host=%s, port=%s)\n", infd, hbuf, sbuf);
          }

          /* Make the incoming socket non-blocking and add it to the
             list of fds to monitor. */
          s = make_socket_non_blocking (infd);
          if (s == -1)
          abort ();

          event.data.fd = infd;
          event.events = EPOLLIN | EPOLLET;
          s = epoll_ctl (efd, EPOLL_CTL_ADD, infd, &event);
          if (s == -1)
          {
            perror ("epoll_ctl");
            abort ();
          }
        }
        continue;
      }
      else
      {
        /* We have data on the fd waiting to be read. Read and
        display it. We must read whatever data is available
        completely, as we are running in edge-triggered mode
        and won't get a notification again for the same
        data. */
        int done = 0;

        while (1)
        {
          ssize_t count;
          char buf[512];

          count = read (events[i].data.fd, buf, sizeof(buf));
          if (count == -1)
          {
            /* If errno == EAGAIN, that means we have read all
               data. So go back to the main loop. */
            if (errno != EAGAIN)
            {
              perror ("read");
              done = 1;
            }
            break;
          }
          else if (count == 0)
          {
            /* End of file. The remote has closed the
             connection. */
            done = 1;
            break;
          }

          /* Write the buffer to standard output */
          // s = write (1, buf, count);
          // if (s == -1)
          //   {
          //     perror ("write");
          //     abort ();
          //   }

          string message = char_to_str(buf, count);
          
          vector<string> token = split(message, '&');

          if (token[0].compare("matching") == 0)
          {
            vector<string> username = split(token[1], '=');
            vector<string> password = split(token[2], '=');
            password[1] = password[1].substr(0, password[1].length() - 1);
                     
            if (mc.isPasswordMatched(username[1], password[1]))
            {
              send(events[i].data.fd, "true\n", 4, 0);
            }
            else
            {
              send(events[i].data.fd, "false\n", 5, 0);
            }           
          }
          else if (token[0].compare("retrieve") == 0)
          {
            vector<string> username = split(token[1], '=');
            username[1] = username[1].substr(0, username[1].length() - 1);

            if (mc.hasUser(username[1])) {
              vector<string> info = mc.retrieveUserInfo(username[1]);
              string reply = "true&" + info[0] + "&" + info[1];
              send(events[i].data.fd, reply.c_str(), reply.size(),0);
            } else {
              //Error user does not exist;
              send(events[i].data.fd, "User does not exist.\n", 20,0);
            }
          }
          else if (token[0].compare("update_profile_link") == 0)
          {
            vector<string> username = split(token[1], '=');
            vector<string> profile_link = split(token[2], '=');
            profile_link[1] = profile_link[1].substr(0, profile_link[1].length() - 1);

            if (mc.hasUser(username[1]))
            {
              if (mc.updateProfileLink(username[1], profile_link[1])) 
              {
                send(events[i].data.fd, "true\n", 4, 0);
              } 
              else 
              {
                send(events[i].data.fd, "false\n", 5, 0);
              }

            } else {
              //Error user does not exist;
              send(events[i].data.fd, "User does not exist.\n", 20,0);
            }
          }
          else if (token[0].compare("update_nickname") == 0)
          {
            vector<string> username = split(token[1], '=');
            vector<string> nickname = split(token[2], '=');
            nickname[1] = nickname[1].substr(0, nickname[1].length() - 1);

            if (mc.hasUser(username[1]))
            {
              if (mc.updateNickname(username[1], nickname[1]))
              {
                send(events[i].data.fd, "true\n", 4, 0);
              }
              else
              {
                send(events[i].data.fd, "false\n", 5, 0);
              }
            } 

            else 
            {
              //Error user does not exist;
              send(events[i].data.fd, "User does not exist.\n", 20,0);
            }
          }
          else
          {
            //Error
            send(events[i].data.fd, "Operation error.\n", 16,0);
          }

        }

        if (done)
        {
          printf ("Closed connection on descriptor %d\n",
          events[i].data.fd);

          /* Closing the descriptor will make epoll remove it
             from the set of descriptors which are monitored. */
          close (events[i].data.fd);
        }
      }
    }
  }

  free (events);

  mc.close();
  close (sfd);

  return EXIT_SUCCESS;
}