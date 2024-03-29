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
#include <iostream>
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
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <libgen.h>
#include <dirent.h>
#include <stdint.h>
#include <sys/file.h>
#include <openssl/md5.h>


#include "../include/ClientList.hpp"
#include "CircularBuffer.hpp"

#define FILE_NOT_FOUND -1
#define FILE_UP_TO_DATE -2
#define READ_BUFF_S 80

#define TRUE             1
#define FALSE            0

#define HASH_PLACE_HOLDER "00000000000000000000000000000000"

struct worker_t_arguments {
    ClientList *first ;
    CircularBuffer *second ;
};

class Client {
private:
    long ip ; // This clients ip in long form

    ClientList list ;
    CircularBuffer *buffer ;
    char *dirName ;
    short portNum ;
    int workerThreads ;
    pthread_t *wthreads;
    int bufferSize ;
    short serverPort ;
    char *serverIP ;
    int countFiles(char *);
    int send_file_list(int );
    int sendFilesInDir(char *,int );
    int send_file(int ,char *,char *);
public:
    Client(char *,short ,int ,int ,short ,char *);
    int connectToserver(void);
    void printInfo(void);
    ~Client();
protected:

};


#endif /* Client_hpp */
