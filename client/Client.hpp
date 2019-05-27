//
//  Client.hpp
//  SYSPRO3
//
//  Created by <author> on 24/05/2019.
//
//

#ifndef Client_hpp
#define Client_hpp

#include <stdio.h>
#include <string.h>
#include <sys/types.h>	     /* sockets */
#include <sys/socket.h>	     /* sockets */
#include <netinet/in.h>	     /* internet sockets */
#include <unistd.h>          /* read, write, close */
#include <netdb.h>	         /* gethostbyaddr */
#include <stdlib.h>	         /* exit */
#include <string.h>	         /* strlen */
#include <limits.h>
#include <time.h>

class Client {
private:
    char *dirName ;
    short portNum ;
    int workerThreads ;
    int bufferSize ;
    short serverPort ;
    char *serverIP ;
public:
    Client(char *,short ,int ,int ,short ,char *);
    int connectToserver(void);
    void printInfo(void);
    ~Client();
protected:

};


#endif /* Client_hpp */
