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
    ClientList() : size(0) , head(NULL) {};
    void addNode(long nip,short nport ) {
        if (head == NULL) {
            struct Node *node = new Node ;
            node->ip = nip ;
            node->port = nport ;
            node->next = NULL ;
            head = node ;
            tail = head ;
            size++;
            return ;
        } else {
            struct Node *node = new Node ;
            node->ip = nip ;
            node->port = nport ;
            node->next = NULL ;
            tail->next = node ;
            tail = node ;
            size++;
            return ;
        }
    }
    void print(void) {
        struct Node *ind = head ;
        int client = 0 ;
        printf("------------------\n");
        while (ind != NULL) {
            printf("Client %d <%ld:%d> \n",client, ind->ip,ind->port);
            client++;
            ind = ind->next ;
        }
        printf("------------------\n" );

    }
    ~ClientList() {}
protected:

};


#endif /* ClientList_hpp */
