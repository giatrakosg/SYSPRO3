//
//  Client.cpp
//  SYSPRO3
//
//  Created by <author> on 24/05/2019.
//
//

#include "Client.hpp"
void perror_exit(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}
void *worker_thread_f(void *_args){ /* Thread function */
    worker_t_arguments *args = (struct worker_t_arguments *)_args ;
    ClientList *list = args->first ;
    CircularBuffer *buffer = args->second ;
    long ip ;
    short port ;
    char *path ;
    char *ver ;
    while (1) {
        buffer->get(ip,port,path,ver);
        printf("Thread %d : Retrieved %ld %d %s %s\n",pthread_self(),ip,port,path,ver );
        delete path ;
        delete ver ;
    }
}


Client::Client(char *dir,short port,int worker,int buff,short sport,char *sip) : bufferSize(buff) {
    dirName = new char [strlen(dir) + 1];
    strcpy(dirName,dir);
    portNum = port ;
    workerThreads = worker ;
    serverPort  = sport ;
    serverIP = new char[strlen(sip) + 1];
    strcpy(serverIP,sip);
    buffer = new CircularBuffer(bufferSize);

}
int Client::send_file_list(int sockfd) {
    
}
int Client::connectToserver(void) {
    srand(time(NULL));
    int             port, sock, i;
    char            buf[256] = { '\0' };

    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*)&server;
    struct hostent *rem;
	/* Create socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    	perror_exit("socket");
	/* Find server address */
    if ((rem = gethostbyname(serverIP)) == NULL) {
	   herror("gethostbyname"); exit(1);
    }
    server.sin_family = AF_INET;       /* Internet domain */
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(serverPort);         /* Server port */
    /* Initiate connection */
    if (connect(sock, serverptr, sizeof(server)) < 0)
	   perror_exit("connect");
    printf("Connecting to %s port %d\n", serverIP, serverPort);
    char command_log_on[17] = {'\0'};
    char command_get_clients[17] = {'\0'} ;
    strcpy(command_log_on,"LOG_ON          ");
    strcpy(command_get_clients,"GET_CLIENTS     ");

    write(sock,command_log_on,17);
    long ip ;

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


    ip=inet_addr(IPbuffer);
    short myport = htons(portNum);
    write(sock,&ip,sizeof(long));
    write(sock,&myport,sizeof(short));
    write(sock,command_get_clients,17);
    char cmd[17] ;
    recv(sock,cmd,17,0); // Receive client list
    printf("Received command : %s\n",cmd );
    int size ;
    recv(sock,&size,sizeof(int),0);
    printf("Received size : %d\n",size );
    long *ips = new long[size];
    short *ports = new short[size];

    //char cmd_log_off[17] ;
    //strcpy(cmd_log_off,"LOG_OFF         ");
    //send(sock,cmd_log_off,17,0);
    pthread_t wthreads[workerThreads];
    struct worker_t_arguments *args = new struct worker_t_arguments ;
    args->first = &list ;
    args->second = buffer ;
    int err ;
    for (int i = 0; i < workerThreads; i++) {
        if (err=pthread_create(&wthreads[i],NULL,worker_thread_f,(void *)args)) {
            /* Create threads 0, 1, 2 */
            perror2("pthread_create", err); exit(1);
            }
    }
    for (int i = 0; i < size; i++) {
        recv(sock,&(ips[i]),sizeof(long),0);
        recv(sock,&(ports[i]),sizeof(short),0);
        ips[i] = ntohl(ips[i]) ;
        ports[i] = ntohs(ports[i]) ;

        // Add the data on our clientlist
        list.addNode(ips[i],ports[i]);
        buffer->put(ips[i],ports[i],NULL,NULL);
    }

    //pause();
    list.print();
    close(sock);                 /* Close socket and exit */


    int    len, rc, on = 1;
    int    listen_sd = -1, new_sd = -1;
    int    desc_ready, end_server = FALSE, compress_array = FALSE;
    int    close_conn;
    struct sockaddr_in6   addr;
    int    timeout;
    struct pollfd fds[200];
    int    nfds = 1, current_size = 0, j;

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
    addr.sin6_port        = htons(portNum);
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
          char cmd[17];
          rc = recv(fds[i].fd, cmd, 17, 0);
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
          len = rc;

          if (strcmp(cmd,"USER_ON         ") == 0) {
              long  recvip ;
              short recvport ;
              rc = recv(fds[i].fd, &recvip, sizeof(long), 0);
              recvip = ntohl(recvip);
              if (rc < 0)
              {
                if (errno != EWOULDBLOCK)
                {
                  perror("  recv() failed");
                  close_conn = TRUE;
                }
                break;
              }
              rc = recv(fds[i].fd, &recvport, sizeof(port), 0);
              recvport = ntohs(recvport);
              if (rc < 0)
              {
                if (errno != EWOULDBLOCK)
                {
                  perror("  recv() failed");
                  close_conn = TRUE;
                }
                break;
              }
              printf("Received %s : <%ld,%d> \n",cmd,recvip,recvport );
              list.addNode(recvip,recvport);
              list.print();
              buffer->put(recvip,recvport,NULL,NULL);
          } else if (strcmp(cmd,"GET_FILE_LIST   ") == 0) {
              send_file_list(fds[i].fd);
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

    return 0 ;

}
void Client::printInfo(void) {
    printf("%s %d %d %d %d %s \n",dirName,portNum,workerThreads,bufferSize,serverPort,serverIP );
}
Client::~Client() {}
