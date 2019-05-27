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


Client::Client(char *dir,short port,int worker,int buffer,short sport,char *sip)  {
    dirName = new char [strlen(dir) + 1];
    strcpy(dirName,dir);
    portNum = port ;
    workerThreads = worker ;
    bufferSize = buffer ;
    serverPort  = sport ;
    serverIP = new char[strlen(sip) + 1];
    strcpy(serverIP,sip);


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
    printf("Connecting to %s port %d\n", serverIP, port);
    do {
    	printf("Give input string: ");
    	fgets(buf, sizeof(buf), stdin);	/* Read from stdin*/
        buf[strcspn(buf, "\r\n")] = 0;
        char cmd[17] = {'\0'};
        strncpy(cmd,buf,16);
        write(sock,cmd,17);
        long ip = htonl(rand() % LONG_MAX);
        short port = htons(rand() % SHRT_MAX );
        write(sock,&ip,sizeof(long));
        write(sock,&port,sizeof(short));
        //read(sock,buf,sizeof(buf));

    	//for(i=0; buf[i] != '\0'; i++) { /* For every char */
    	    /* Send i-th character */
        //	if (write(sock, buf + i, 1) < 0)
        //	   perror_exit("write");
            /* receive i-th character transformed */
        //	if (read(sock, buf + i, 1) < 0)
        //	    perror_exit("read");
    	//}
    } while (strcmp(buf, "END") != 0); /* Finish on "end" */
    close(sock);                 /* Close socket and exit */
    return 0 ;

}
void Client::printInfo(void) {
    printf("%s %d %d %d %d %s \n",dirName,portNum,workerThreads,bufferSize,serverPort,serverIP );
}
Client::~Client() {}
