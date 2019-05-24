//
//  ClientList.hpp
//  SYSPRO3
//
//  Created by <author> on 22/05/2019.
//
//

#ifndef ClientList_hpp
#define ClientList_hpp

#include <stdio.h>
#include <sys/socket.h>	     /* sockets */
#include <netinet/in.h>	     /* internet sockets */
#include <arpa/inet.h>
#include <string.h>


struct Node {
    long ip ;
    short port ;
    struct Node *next ;
};

class ClientList {
private:

public:
    int size ;
    struct Node *head ;
    struct Node *tail ;
    ClientList();
    void addNode(long ,short );
    void print(void);
    ~ClientList();
protected:

};


#endif /* ClientList_hpp */
