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
    buffer->get(ip,port,path,ver);
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
int Client::connectToserver(void) {
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

    for (int i = 0; i < size; i++) {
        recv(sock,&(ips[i]),sizeof(long),0);
        recv(sock,&(ports[i]),sizeof(short),0);
        ips[i] = ntohl(ips[i]) ;
        ports[i] = ntohs(ports[i]) ;

        // Add the data on our clientlist
        list.addNode(ips[i],ports[i]);
    }
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


    close(sock);                 /* Close socket and exit */

    return 0 ;

}
void Client::printInfo(void) {
    printf("%s %d %d %d %d %s \n",dirName,portNum,workerThreads,bufferSize,serverPort,serverIP );
}
Client::~Client() {}
