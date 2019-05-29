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
int Server::first_connection(int sock_fd) {
    list.print();
    char buffer[17] ;
    char cmd[17] ; // Buffer to store the command

    unsigned long ip ;
    unsigned short port ;
    int rc ;
    /*****************************************************/
    /* Data was received                                 */
    /*****************************************************/
    rc = recv(sock_fd, &ip, sizeof(long), 0);
    if (rc < 0)
    {
      if (errno != EWOULDBLOCK)
      {
        perror("  recv() failed");
        close_conn = TRUE;
      }
      return -1 ;
    }
    /*****************************************************/
    /* Data was received                                 */
    /*****************************************************/
    rc = recv(sock_fd, &port, sizeof(short), 0);
    if (rc < 0)
    {
      if (errno != EWOULDBLOCK)
      {
        perror("  recv() failed");
        close_conn = TRUE;
      }
      return -1 ;
    }


    // Send user_on command to each client
    struct Node *ind = list.head ;
    while (ind != NULL) {
        send_user_on(ind->ip,ind->port,ip,port);
        ind = ind->next;
    }

    // Store in network order
    ip = ntohl(ip);
    port = ntohs(port);
    list.addNode(ip,port);

    // Receive get_clients command over same connection
    while(1) {
        rc = recv(sock_fd, buffer,17, 0);
        if (rc < 0)
        {
            if (errno != EWOULDBLOCK)
            {
                perror("  recv() failed");
                close_conn = TRUE;
            }
            return -1 ;
        } else if (rc > 0) {
            break ;
        }
    }

    strcpy(cmd,buffer);
    if (strcmp(cmd,"GET_CLIENTS     ") == 0) {
        printf("Received Get_Clients\n");
    }
    return 0 ;

}
int Server::send_user_on(long sendip,short sendport,long usrip,short usrport) {
    return 0 ;


}
int Server::send_get_clients(int socketfd) {
    return 0 ;
}

void Server::run_server(void) {
    char hostbuffer[256];
	char *IPbuffer;
	struct hostent *host_entry;
	int hostname;

    hostname = gethostname(hostbuffer, sizeof(hostbuffer));

	// To retrieve host information
	host_entry = gethostbyname(hostbuffer);
    if (host_entry == NULL)
	{
		perror("gethostbyname");
		exit(1);
	}

	// To convert an Internet network
	// address into ASCII string
	IPbuffer = inet_ntoa(*((struct in_addr*)
						host_entry->h_addr_list[0]));
                        if (NULL == IPbuffer)
                    	{
                    		perror("inet_ntoa");
                    		exit(1);
                    	}
    printf("Server IP : %s\n",IPbuffer );



      int    rc, on = 1;
      int    listen_sd = -1, new_sd = -1;
      int    end_server = FALSE, compress_array = FALSE;

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
              char buffer[17] = {'\0'} ;
              char cmd[17] = {'\0'}; // Buffer to store the command
              rc = recv(fds[i].fd, buffer,17, 0);
              if (rc < 0)
              {
                if (errno != EWOULDBLOCK)
                {
                  perror("  recv() failed");
                  close_conn = TRUE;
                }
                break;
              }
              if (rc == 0)
              {
                printf("  Connection closed\n");
                close_conn = TRUE;
                break;
              }

              strcpy(cmd,buffer);
              if (cmd[0] != NULL) {
                  printf("Command : %s \n",cmd);
              }

              //list.addNode(ip,port);
              if (strcmp(cmd,"LOG_ON          ") == 0) {
                  first_connection(fds[i].fd);
              }
              else if (strcmp(cmd,"LOG_OFF         ") == 0) {
                  struct sockaddr_in addr;
                  socklen_t addr_size = sizeof(struct sockaddr_in);
                  if(getpeername(fds[i].fd, (struct sockaddr *)&addr, &addr_size) <  0) {
                      perror("getpeername");
                  }
                  char *clientip = new char[20];
                  strcpy(clientip, inet_ntoa(addr.sin_addr));
                  //list.addNode(addr);
                  printf("Accepted connection from %s:%d\n", clientip,addr.sin_port);

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
