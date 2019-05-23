/* inet_str_client.c: Internet stream sockets client */
#include <stdio.h>
#include <sys/types.h>	     /* sockets */
#include <sys/socket.h>	     /* sockets */
#include <netinet/in.h>	     /* internet sockets */
#include <unistd.h>          /* read, write, close */
#include <netdb.h>	         /* gethostbyaddr */
#include <stdlib.h>	         /* exit */
#include <string.h>	         /* strlen */
#include <limits.h>
#include <time.h>

void perror_exit(char *message);

int main(int argc, char *argv[]) {
    srand(time(NULL));
    int             port, sock, i;
    char            buf[256] = { '\0' };

    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*)&server;
    struct hostent *rem;

    if (argc != 3) {
    	printf("Please give host name and port number\n");
       	exit(1);}
	/* Create socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    	perror_exit("socket");
	/* Find server address */
    if ((rem = gethostbyname(argv[1])) == NULL) {
	   herror("gethostbyname"); exit(1);
    }
    port = atoi(argv[2]); /*Convert port number to integer*/
    server.sin_family = AF_INET;       /* Internet domain */
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(port);         /* Server port */
    /* Initiate connection */
    if (connect(sock, serverptr, sizeof(server)) < 0)
	   perror_exit("connect");
    printf("Connecting to %s port %d\n", argv[1], port);
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

void perror_exit(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}
