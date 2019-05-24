//
//  Server.cpp
//  SYSPRO3
//
//  Created by <author> on 22/05/2019.
//
//

#include "Server.hpp"


void child_server(int newsock);
void perror_exit(char *message);
void sigchld_handler (int sig);
int Server::send_user_on(long sendip,short sendport,long usrip,short usrport) {}
int Server::send_get_client(int socketfd) {}
void Server::run_server(void) {
    int sock, newsock;
    struct sockaddr_in server, client;
    socklen_t clientlen;

    struct sockaddr *serverptr=(struct sockaddr *)&server;
    struct sockaddr *clientptr=(struct sockaddr *)&client;
    struct hostent *rem;

    /* Reap dead children asynchronously */
    signal(SIGCHLD, sigchld_handler);
    /* Create socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror_exit("socket");

    server.sin_family = AF_INET;       /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);      /* The given port */

    /* Bind socket to address */
    if (bind(sock, serverptr, sizeof(server)) < 0)
        perror_exit("bind");

    /* Listen for connections */
    if (listen(sock, 5) < 0) perror_exit("listen");
    printf("Listening for connections to port %d\n", port);
    while (1) {
        /* accept connection */
    	if ((newsock = accept(sock, clientptr, &clientlen)) < 0)
            perror_exit("accept");

        // Gets clients ip address and port
        // Segment taken from
        //https://stackoverflow.com/questions/20472072/c-socket-get-ip-address-from-filedescriptor-returned-from-accept/20475352
        struct sockaddr_in addr;
        socklen_t addr_size = sizeof(struct sockaddr_in);
        int res = getpeername(newsock, (struct sockaddr *)&addr, &addr_size);
        char *clientip = new char[20];
        strcpy(clientip, inet_ntoa(addr.sin_addr));
        //list.addNode(addr);
    	printf("Accepted connection from %s:%d\n", clientip,addr.sin_port);
        /*
    	switch (fork()) {
        	case -1:
        	    perror("fork"); break;
        	case 0:
        	    close(sock); child_server(newsock);
        	    exit(0);
    	}      */

    	close(newsock); /* parent closes socket to client            */
			/* must be closed before it gets re-assigned */
        //list.print();
    }
}
void Server::run_ibm_server(void) {

      int    len, rc, on = 1;
      int    listen_sd = -1, new_sd = -1;
      int    desc_ready, end_server = FALSE, compress_array = FALSE;
      int    close_conn;
      char   buffer[80];
      struct sockaddr_in6   addr;
      int    timeout;
      struct pollfd fds[200];
      int    nfds = 1, current_size = 0, i, j;

      /*************************************************************/
      /* Create an AF_INET6 stream socket to receive incoming      */
      /* connections on                                            */
      /*************************************************************/
      listen_sd = socket(AF_INET6, SOCK_STREAM, 0);
      if (listen_sd < 0)
      {
        perror("socket() failed");
        exit(-1);
      }

      /*************************************************************/
      /* Allow socket descriptor to be reuseable                   */
      /*************************************************************/
      rc = setsockopt(listen_sd, SOL_SOCKET,  SO_REUSEADDR,
                      (char *)&on, sizeof(on));
      if (rc < 0)
      {
        perror("setsockopt() failed");
        close(listen_sd);
        exit(-1);
      }

      /*************************************************************/
      /* Set socket to be nonblocking. All of the sockets for      */
      /* the incoming connections will also be nonblocking since   */
      /* they will inherit that state from the listening socket.   */
      /*************************************************************/
      rc = ioctl(listen_sd, FIONBIO, (char *)&on);
      if (rc < 0)
      {
        perror("ioctl() failed");
        close(listen_sd);
        exit(-1);
      }

      /*************************************************************/
      /* Bind the socket                                           */
      /*************************************************************/
      memset(&addr, 0, sizeof(addr));
      addr.sin6_family      = AF_INET6;
      memcpy(&addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
      addr.sin6_port        = htons(port);
      rc = bind(listen_sd,
                (struct sockaddr *)&addr, sizeof(addr));
      if (rc < 0)
      {
        perror("bind() failed");
        close(listen_sd);
        exit(-1);
      }

      /*************************************************************/
      /* Set the listen back log                                   */
      /*************************************************************/
      rc = listen(listen_sd, 32);
      if (rc < 0)
      {
        perror("listen() failed");
        close(listen_sd);
        exit(-1);
      }

      /*************************************************************/
      /* Initialize the pollfd structure                           */
      /*************************************************************/
      memset(fds, 0 , sizeof(fds));

      /*************************************************************/
      /* Set up the initial listening socket                        */
      /*************************************************************/
      fds[0].fd = listen_sd;
      fds[0].events = POLLIN;
      /*************************************************************/
      /* Initialize the timeout to 3 minutes. If no                */
      /* activity after 3 minutes this program will end.           */
      /* timeout value is based on milliseconds.                   */
      /*************************************************************/
      timeout = (3 * 60 * 1000);

      /*************************************************************/
      /* Loop waiting for incoming connects or for incoming data   */
      /* on any of the connected sockets.                          */
      /*************************************************************/
      do
      {
        /***********************************************************/
        /* Call poll() and wait 3 minutes for it to complete.      */
        /***********************************************************/
        printf("Waiting on poll()...\n");
        rc = poll(fds, nfds, timeout);

        /***********************************************************/
        /* Check to see if the poll call failed.                   */
        /***********************************************************/
        if (rc < 0)
        {
          perror("  poll() failed");
          break;
        }

        /***********************************************************/
        /* Check to see if the 3 minute time out expired.          */
        /***********************************************************/
        if (rc == 0)
        {
          printf("  poll() timed out.  End program.\n");
          break;
        }


        /***********************************************************/
        /* One or more descriptors are readable.  Need to          */
        /* determine which ones they are.                          */
        /***********************************************************/
        current_size = nfds;
        for (i = 0; i < current_size; i++)
        {
          /*********************************************************/
          /* Loop through to find the descriptors that returned    */
          /* POLLIN and determine whether it's the listening       */
          /* or the active connection.                             */
          /*********************************************************/
          if(fds[i].revents == 0)
            continue;

          /*********************************************************/
          /* If revents is not POLLIN, it's an unexpected result,  */
          /* log and end the server.                               */
          /*********************************************************/
          if(fds[i].revents != POLLIN)
          {
            printf("  Error! revents = %d\n", fds[i].revents);
            end_server = TRUE;
            break;

          }
          if (fds[i].fd == listen_sd)
          {
            /*******************************************************/
            /* Listening descriptor is readable.                   */
            /*******************************************************/
            printf("  Listening socket is readable\n");

            /*******************************************************/
            /* Accept all incoming connections that are            */
            /* queued up on the listening socket before we         */
            /* loop back and call poll again.                      */
            /*******************************************************/
            do
            {
              /*****************************************************/
              /* Accept each incoming connection. If               */
              /* accept fails with EWOULDBLOCK, then we            */
              /* have accepted all of them. Any other              */
              /* failure on accept will cause us to end the        */
              /* server.                                           */
              /*****************************************************/
              new_sd = accept(listen_sd, NULL, NULL);
              if (new_sd < 0)
              {
                if (errno != EWOULDBLOCK)
                {
                  perror("  accept() failed");
                  end_server = TRUE;
                }
                break;
              }

              /*****************************************************/
              /* Add the new incoming connection to the            */
              /* pollfd structure                                  */
              /*****************************************************/
              printf("  New incoming connection - %d\n", new_sd);
              fds[nfds].fd = new_sd;
              fds[nfds].events = POLLIN;
              nfds++;

              /*****************************************************/
              /* Loop back up and accept another incoming          */
              /* connection                                        */
              /*****************************************************/
            } while (new_sd != -1);
          }

          /*********************************************************/
          /* This is not the listening socket, therefore an        */
          /* existing connection must be readable                  */
          /*********************************************************/

          else
          {
            printf("  Descriptor %d is readable\n", fds[i].fd);
            close_conn = FALSE;
            /*******************************************************/
            /* Receive all incoming data on this socket            */
            /* before we loop back and call poll again.            */
            /*******************************************************/

            do
            {

              /*****************************************************/
              /* Receive data on this connection until the         */
              /* recv fails with EWOULDBLOCK. If any other         */
              /* failure occurs, we will close the                 */
              /* connection.                                       */
              /*****************************************************/
              list.print();
              char buffer[17] ;
              char cmd[17] ; // Buffer to store the command
              long ip ;
              short port ;
              rc = recv(fds[i].fd, buffer,17, 0);
              strcpy(cmd,buffer);
              if (rc < 0)
              {
                if (errno != EWOULDBLOCK)
                {
                  perror("  recv() failed");
                  close_conn = TRUE;
                }
                break;
              }

              /*****************************************************/
              /* Check to see if the connection has been           */
              /* closed by the client                              */
              /*****************************************************/
              if (rc == 0)
              {
                printf("  Connection closed\n");
                close_conn = TRUE;
                break;
              }
              strcpy(cmd,buffer);
              /*****************************************************/
              /* Data was received                                 */
              /*****************************************************/
              rc = recv(fds[i].fd, &ip, sizeof(long), 0);
              if (rc < 0)
              {
                if (errno != EWOULDBLOCK)
                {
                  perror("  recv() failed");
                  close_conn = TRUE;
                }
                break;
              }

              /*****************************************************/
              /* Check to see if the connection has been           */
              /* closed by the client                              */
              /*****************************************************/
              if (rc == 0)
              {
                printf("  Connection closed\n");
                close_conn = TRUE;
                break;
              }

              /*****************************************************/
              /* Data was received                                 */
              /*****************************************************/
              rc = recv(fds[i].fd, &port, sizeof(short), 0);
              if (rc < 0)
              {
                if (errno != EWOULDBLOCK)
                {
                  perror("  recv() failed");
                  close_conn = TRUE;
                }
                break;
              }

              /*****************************************************/
              /* Check to see if the connection has been           */
              /* closed by the client                              */
              /*****************************************************/
              if (rc == 0)
              {
                printf("  Connection closed\n");
                close_conn = TRUE;
                break;
              }

              /*****************************************************/
              /* Data was received                                 */
              /*****************************************************/
              /*****************************************************/
              /* Echo the data back to the client                  */
              /*****************************************************/
              rc = send(fds[i].fd, buffer, len, 0);
              if (rc < 0)
              {
                perror("  send() failed");
                close_conn = TRUE;
                break;
              }
              ip = ntohl(ip);
              port = ntohs(port);
              //list.addNode(ip,port);
              printf("Command : %s , IP : %ld , Port : %d\n",cmd,ip,port );
              if (strcmp(cmd,"LOG_ON          ") == 0) {
                  struct Node *ind = list.head ;
                  while (ind != NULL) {
                      send_user_on(ind->ip,ind->port,ip,port);
                  }
                  list.addNode(ip,port);
              } else if (strcmp(cmd,"GET_CLIENTS     ") == 0) {
                  send_get_clients(fds[i].fd);
              }
            } while(TRUE);

            /*******************************************************/
            /* If the close_conn flag was turned on, we need       */
            /* to clean up this active connection. This            */
            /* clean up process includes removing the              */
            /* descriptor.                                         */
            /*******************************************************/
            if (close_conn)
            {
              close(fds[i].fd);
              fds[i].fd = -1;
              compress_array = TRUE;
            }


          }  /* End of existing connection is readable             */
        } /* End of loop through pollable descriptors              */

        /***********************************************************/
        /* If the compress_array flag was turned on, we need       */
        /* to squeeze together the array and decrement the number  */
        /* of file descriptors. We do not need to move back the    */
        /* events and revents fields because the events will always*/
        /* be POLLIN in this case, and revents is output.          */
        /***********************************************************/
        if (compress_array)
        {
          compress_array = FALSE;
          for (i = 0; i < nfds; i++)
          {
            if (fds[i].fd == -1)
            {
              for(j = i; j < nfds; j++)
              {
                fds[j].fd = fds[j+1].fd;
              }
              i--;
              nfds--;
            }
          }
        }

      } while (end_server == FALSE); /* End of serving running.    */

      /*************************************************************/
      /* Clean up all of the sockets that are open                 */
      /*************************************************************/
      for (i = 0; i < nfds; i++)
      {
        if(fds[i].fd >= 0)
          close(fds[i].fd);
      }
}
void child_server(int newsock) {
    char buf[1];
    while(read(newsock, buf, 1) > 0) {  /* Receive 1 char */
    	putchar(buf[0]);           /* Print received char */
    	/* Capitalize character */
    	buf[0] = toupper(buf[0]);
    	/* Reply */
    	if (write(newsock, buf, 1) < 0)
    	    perror_exit("write");
    }
    printf("Closing connection.\n");
    close(newsock);	  /* Close socket */
}

/* Wait for all dead child processes */
void sigchld_handler (int sig) {
	while (waitpid(-1, NULL, WNOHANG) > 0);
}

void perror_exit(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}


Server::Server(int port) : port(port) {}
Server::~Server() {}
