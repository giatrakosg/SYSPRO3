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
    void printInfo(void);
    ~Client();
protected:

};


#endif /* Client_hpp */
