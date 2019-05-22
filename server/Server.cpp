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
    	if ((newsock = accept(sock, clientptr, &clientlen)) < 0) perror_exit("accept");

        // Gets clients ip address and port
        // Segment taken from https://stackoverflow.com/questions/20472072/c-socket-get-ip-address-from-filedescriptor-returned-from-accept/20475352
        struct sockaddr_in addr;
        socklen_t addr_size = sizeof(struct sockaddr_in);
        int res = getpeername(newsock, (struct sockaddr *)&addr, &addr_size);
        char *clientip = new char[20];
        strcpy(clientip, inet_ntoa(addr.sin_addr));
        list.addNode(addr);
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
        list.print();
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
