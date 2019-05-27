//
//  Client.cpp
//  SYSPRO3
//
//  Created by <author> on 24/05/2019.
//
//

#include "Client.hpp"

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
void Client::printInfo(void) {
    printf("%s %d %d %d %d %s \n",dirName,portNum,workerThreads,bufferSize,serverPort,serverIP );
}
Client::~Client() {}
