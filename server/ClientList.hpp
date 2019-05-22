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
    struct sockaddr_in data ;
    struct Node *next ;
};

class ClientList {
private:
    int size ;
    struct Node *head ;
    struct Node *tail ;

public:
    ClientList();
    void addNode(struct sockaddr_in );
    void print(void);
    ~ClientList();
protected:

};


#endif /* ClientList_hpp */
