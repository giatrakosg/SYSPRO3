//
//  Client.cpp
//  SYSPRO3
//
//  Created by <author> on 24/05/2019.
//
//

#include "Client.hpp"
void calculatemd5hash(char *,char *&);
int file_exist(char *);


void perror_exit(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}


// Removes the file name from a path
int mkdir_p(const char *path)
{
    /* Adapted from http://stackoverflow.com/a/2336245/119527 */
    const size_t len = strlen(path);
    char _path[PATH_MAX];
    char *p;

    errno = 0;

    /* Copy string so its mutable */
    if (len > sizeof(_path)-1) {
        errno = ENAMETOOLONG;
        return -1;
    }
    strcpy(_path, path);

    /* Iterate the string */
    for (p = _path + 1; *p; p++) {
        if (*p == '/') {
            /* Temporarily truncate */
            *p = '\0';

            if (mkdir(_path, S_IRWXU) != 0) {}

            *p = '/';
        }
    }

    if (mkdir(_path, S_IRWXU) != 0) {
        if (errno != EEXIST)
            return -1;
    }

    return 0;
}



void *worker_thread_f(void *_args){ /* Thread function */
    worker_t_arguments *args = (struct worker_t_arguments *)_args ;
    ClientList *list = args->first ;
    CircularBuffer *buffer = args->second ;
    long ip ;
    short port ;
    char *path = NULL;
    char *ver = NULL;
    while (1) {
        buffer->get(ip,port,path,ver);
        printf("Thread %d : Retrieved %ld %d %s %s\n",pthread_self(),ip,port,path,ver );
        // The item is not a file so we send a GET_FILE_LIST request
        if ((path == NULL) || (ver == NULL)) {
            sleep(2);
            int             sock;

            struct in_addr ip_addr;
            ip_addr.s_addr = htonl(ip);


            struct sockaddr_in server;
            struct sockaddr *serverptr = (struct sockaddr*)&server;
            struct hostent *rem;


        	/* Create socket */
            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            	perror_exit("socket");
        	/* Find server address */
            if ((rem = gethostbyname(inet_ntoa(ip_addr))) == NULL) {
        	   herror("gethostbyname"); exit(1);
            }

            server.sin_family = AF_INET;       /* Internet domain */
            memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
            server.sin_port = htons(port);         /* Server port */
            /* Initiate connection */
            if (connect(sock, serverptr, sizeof(server)) < 0)
        	   perror_exit("connect");
            char cmd_get_file_list[17] ;
            strcpy(cmd_get_file_list,"GET_FILE_LIST   ");
            write(sock,cmd_get_file_list,17);
            char cmd_recv[17];
            recv(sock,cmd_recv,17,0);
            if (strcmp(cmd_recv,"FILE_LIST       ") == 0) {
                long nfiles ;
                recv(sock,&nfiles,sizeof(long),0);
                nfiles = ntohl(nfiles);
                char path[PATH_LEN + 1];
                char ver[VER_LEN + 1];
                for (int i = 0; i < nfiles; i++) {
                    recv(sock,path,PATH_LEN + 1,0);
                    recv(sock,ver,VER_LEN + 1,0);
                    char clientpath[512];
                    sprintf(clientpath,"mirror%ld%d/%s",ip,port,path);
                    if (file_exist(clientpath)) {
                        char *currversion ;
                        calculatemd5hash(clientpath,currversion);
                        buffer->put(ip,port,path,currversion);
                    } else {
                        buffer->put(ip,port,path,HASH_PLACE_HOLDER);
                    }
                }

            }

            close(sock);                 /* Close socket and exit */

        }
        // The item is a reference to a file
        else {
            int             sock;

            struct in_addr ip_addr;
            ip_addr.s_addr = htonl(ip);


            struct sockaddr_in server;
            struct sockaddr *serverptr = (struct sockaddr*)&server;
            struct hostent *rem;


        	/* Create socket */
            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            	perror_exit("socket");
        	/* Find server address */
            if ((rem = gethostbyname(inet_ntoa(ip_addr))) == NULL) {
        	   herror("gethostbyname"); exit(1);
            }

            server.sin_family = AF_INET;       /* Internet domain */
            memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
            server.sin_port = htons(port);         /* Server port */
            /* Initiate connection */
            if (connect(sock, serverptr, sizeof(server)) < 0)
        	   perror_exit("connect");
            char cmd_get_file[17] ;
            strcpy(cmd_get_file,"GET_FILE        ");
            send(sock,cmd_get_file,17,0);
            send(sock,path,PATH_LEN + 1,0);
            send(sock,ver,VER_LEN + 1,0);

            char cmd_recv[17];
            recv(sock,cmd_recv,17,0);
            if (strcmp(cmd_recv,"FILE_NOT_FOUND  ") == 0) {
                fprintf(stderr, "Error file not found \n");
            }
            else if (strcmp(cmd_recv,"FILE_UP_TO_DATE ") == 0) {
                fprintf(stderr, "File up to date\n");
            }
            else if (strcmp(cmd_recv,"FILE_SIZE       ") == 0) {
                char newversion[VER_LEN + 1];
                long size ;
                recv(sock,newversion,VER_LEN + 1,0);
                recv(sock,&size,sizeof(long),0);
                size = ntohl(size);
                char clientpath[512];
                sprintf(clientpath,"mirror%ld%d/%s",ip,port,path);
                // needed because dirname modifies input
                char clientpath2[512];
                strcpy(clientpath2,clientpath);
                mkdir_p(dirname(clientpath2));
                FILE * outD = fopen(clientpath,"w+");
                int read_bytes ;
                if (outD != NULL) {
                    char *contents = new char[size];
                    memset(contents,'\0',size);
                    // We wait until the client has send all the data
                    //sleep(2);
                    int counter = size ;
                    while (counter > 0) {
                        char *readBuffer = new char[READ_BUFF_S +1]; // We use a buffer to
                        // read over the pipe
                        memset(readBuffer,'\0',READ_BUFF_S+1);
                        read_bytes = recv(sock,readBuffer,READ_BUFF_S ,0);
                        strcat(contents,readBuffer);
                        counter -= read_bytes ;
                        delete readBuffer;
                    }
                    fwrite(contents,sizeof(char),size,outD);
                    fclose(outD);
                    delete contents ;

                } else {
                    perror("fopen");
                }

            }

            close(sock);                 /* Close socket and exit */


        }

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

void calculatemd5hash(char *path,char *&hash) {
    hash = new char[VER_LEN + 1];
    hash[VER_LEN] = '\0';
    unsigned char c[MD5_DIGEST_LENGTH];
    int i;
    FILE *inFile = fopen (path, "rb");
    MD5_CTX mdContext;
    int bytes;
    unsigned char data[1024];

    if (inFile == NULL) {
        printf ("%s can't be opened.\n", path);
        return ;
    }

    MD5_Init (&mdContext);
    while ((bytes = fread (data, 1, 1024, inFile)) != 0)
        MD5_Update (&mdContext, data, bytes);
    MD5_Final (c,&mdContext);
    for(int i = 0; i < 16; ++i)
    sprintf(&hash[i*2], "%02x", (unsigned int)c[i]);

    //for(i = 0; i < MD5_DIGEST_LENGTH; i++) sprintf("%02x", c[i]);
    fclose (inFile);

}



int Client::countFiles(char *dirpath) {
    int sum = 0 ;
    DIR * dir = opendir(dirpath);
    if (dir == NULL) {
        fprintf(stderr, "NULL dir\n");
    }
    //fprintf(logF,"Dirpath sendFilesInDir : %s \n",dirpath);
    struct dirent *ind ;
    while((ind = readdir(dir)) != NULL) {
        //fprintf(stdout,"Dirpath : %s\n",ind->d_name );
        fflush(stdout);
        // We check if it is a dir or a regular file
        // This code snippet was taken from
        // https://stackoverflow.com/questions/4553012/checking-if-a-file-is-a-directory-or-just-a-file
        struct stat path_stat;
        char totalpath[512];
        sprintf(totalpath,"%s/%s",dirpath,ind->d_name);
        stat(totalpath, &path_stat);

        // Ignore . , .. directories
        if ((strcmp(ind->d_name,"..") == 0) || (strcmp(ind->d_name,".") == 0)) {
            continue ;
        }
        // This is a directory
        if(!S_ISREG(path_stat.st_mode)) {
            char path[512] ;
            sprintf(path,"%s/%s",dirpath,ind->d_name);
            sum += countFiles(totalpath);
            continue ;
        }
        sum += 1;
    }
    closedir(dir);
    return sum ;

}


int Client::sendFilesInDir(char *dirpath,int sockfd) {
    DIR * dir = opendir(dirpath);
    if (dir == NULL) {
        fprintf(stderr, "NULL dir\n");
    }
    //fprintf(logF,"Dirpath sendFilesInDir : %s \n",dirpath);
    struct dirent *ind ;
    while((ind = readdir(dir)) != NULL) {
        //fprintf(stdout,"Dirpath : %s\n",ind->d_name );
        fflush(stdout);
        // We check if it is a dir or a regular file
        // This code snippet was taken from
        // https://stackoverflow.com/questions/4553012/checking-if-a-file-is-a-directory-or-just-a-file
        struct stat path_stat;
        char totalpath[512];
        sprintf(totalpath,"%s/%s",dirpath,ind->d_name);
        stat(totalpath, &path_stat);

        // Ignore . , .. directories
        if ((strcmp(ind->d_name,"..") == 0) || (strcmp(ind->d_name,".") == 0)) {
            continue ;
        }
        // This is a directory
        if(!S_ISREG(path_stat.st_mode)) {
            char path[512] ;
            sprintf(path,"%s/%s",dirpath,ind->d_name);
            sendFilesInDir(totalpath,sockfd);
            continue ;
        }
        //fprintf(stdout,"Path : %s\n",totalpath );
        //fflush(logF);
        // We check if it
        char outpath[129];
        char *hash ;
        strcpy(outpath,totalpath);
        calculatemd5hash(totalpath,hash);
        send(sockfd,outpath,PATH_LEN + 1,0);
        send(sockfd,hash,VER_LEN + 1,0);
        delete hash ;
    }
    closedir(dir);
    return 0 ;

}
// Found at
// https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c
int file_exist (char *filename)
{
  struct stat   buffer;
  char totalpath[512];
  sprintf(totalpath,"./%s",filename);
  return (stat (totalpath, &buffer) == 0);
}
int file_size(char *filename) {
    struct stat st;
    stat(filename, &st);
    int size = st.st_size;
    return size ;
}
int Client::send_file_list(int sockfd) {
    char *cmd_file_list = "FILE_LIST       ";
    long nfiles = countFiles(dirName);
    nfiles = htonl(nfiles);
    send(sockfd,cmd_file_list,17,0);
    send(sockfd,&nfiles,sizeof(long),0);
    sendFilesInDir(dirName,sockfd);
}
int Client::send_file(int sockfd,char *path,char *version) {

    if (!file_exist(path))
    {
        char cmd_file_not_found[17];
        strcpy(cmd_file_not_found,"FILE_NOT_FOUND  ");
        send(sockfd,cmd_file_not_found,17,0);
        return FILE_NOT_FOUND ;
    }
    char *currversion ;
    calculatemd5hash(path,currversion);
    if (strcmp(currversion,version) == 0) {
        char cmd_file_up_to_date[17];
        strcpy(cmd_file_up_to_date,"FILE_UP_TO_DATE ");
        send(sockfd,cmd_file_up_to_date,17,0);
        return FILE_UP_TO_DATE ;

    }
    char cmd_file_size[17];
    strcpy(cmd_file_size,"FILE_SIZE       ");
    send(sockfd,cmd_file_size,17,0);
    send(sockfd,currversion,VER_LEN + 1,0);
    delete currversion ;
    long size = file_size(path);
    size = htonl(size);
    send(sockfd,&size,sizeof(long),0);
    // Found at https://stackoverflow.com/questions/174531/how-to-read-the-content-of-a-file-to-a-string-in-c
    int fd = open(path,O_RDONLY);
    if (fd < -1) {
        perror("open");
        return -1 ;
    }
    int count = ntohl(size) ;
    while(count > 0) {
        char *contents = new char[READ_BUFF_S+1];
        memset(contents,'\0',READ_BUFF_S+1);
        read(fd,contents,READ_BUFF_S);
        int err = send(sockfd,contents,READ_BUFF_S,0);
        if (err < 0) {
            perror("send");
        }
        count -= READ_BUFF_S ;
        delete contents ;
    }
    close(fd) ;
    return 0 ;




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

    struct worker_t_arguments *args = new struct worker_t_arguments ;
    args->first = &list ;
    args->second = buffer ;
    int err ;
    wthreads = new pthread_t[workerThreads];
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
          } else if (strcmp(cmd,"GET_FILE        ") == 0) {
              char filepath[PATH_LEN + 1];
              char fileversion[VER_LEN + 1];
              recv(fds[i].fd,filepath,PATH_LEN+1,0);
              recv(fds[i].fd,fileversion,VER_LEN+1,0);
              send_file(fds[i].fd,filepath,fileversion);
          } else if(strcmp(cmd,"USER_OFF        ") == 0) {
              long offip ;  short offport ;
              recv(fds[i].fd,&offip,sizeof(long),0);
              recv(fds[i].fd,&offport,sizeof(short),0);
              offip = ntohl(offip);
              offport = ntohs(offport);
              printf("Received log_off <%ld,%d> \n",offip,offport );
              list.remove(offip,offport);

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
Client::~Client() {
    int             port, sock, i;
    char            buf[256] = { '\0' };

    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*)&server;
    struct hostent *rem;



	/* Create socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    	perror_exit("socket");
	/* Find server address */

    struct sockaddr_in6   addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family      = AF_INET6;
    memcpy(&addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
    addr.sin6_port        = htons(portNum);
    bind(sock,
              (struct sockaddr *)&addr, sizeof(addr));


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
    char command_log_off[17] = {'\0'};
    strcpy(command_log_off,"LOG_OFF         ");
    send(sock,command_log_off,17,0);

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

    close(sock);
    for (int i = 0; i < workerThreads; i++) {
        pthread_kill(wthreads[i],SIGTERM);
    }
    delete wthreads ;

    delete buffer ;
    printf("Deleting dirName \n");
    delete dirName ;
    printf("Deleting ServerIP\n");
    delete serverIP ;


}
