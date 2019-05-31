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
#define NO_NODE -1 ;
#define DUP_NODE -2 ;
struct Node {
    long ip ;
    short port ;
    struct Node *next ;
};

class ClientList {
private:
    bool ismember(long fip,short fport) {
        struct Node *ind = head ;
        while (ind != NULL) {
            if ((ind->ip == fip) && (ind->port == fport)) {
                return true ;
            }
            ind = ind->next ;
        }
        return false ;
    }
public:
    int size ;
    struct Node *head ;
    struct Node *tail ;
    ClientList() : size(0) , head(NULL) {};
    int addNode(long nip,short nport ) {
        // If a node with same data already
        if (ismember(nip,nport)) {
            return DUP_NODE ;
        }
        if (head == NULL) {
            struct Node *node = new Node ;
            node->ip = nip ;
            node->port = nport ;
            node->next = NULL ;
            head = node ;
            tail = head ;
            size++;
            return 0;
        } else {
            struct Node *node = new Node ;
            node->ip = nip ;
            node->port = nport ;
            node->next = NULL ;
            tail->next = node ;
            tail = node ;
            size++;
            return 0 ;
        }
        return 0 ;
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
    int remove(long rip,short rport) {
        // If there is no node with this data then return -1
        if (!ismember(rip,rport)) {
            return NO_NODE ;
        }

        struct Node *it = head->next ;
        struct Node *prev = head ;
        // The is only one entry on the list
        // The coin is in the top of the list
        if ((head->ip == rip) && (head->port == rport)) {
            if (head == tail) {
                tail = head->next ;
            }
            head = head->next ;
            return 0;
        }
        if (it == NULL) {
            if ((prev->ip == rip) && (prev->port == rport)) {
                head  = NULL ;
                tail = NULL ;
            }
            return 0;
        }
        while (it != NULL) {
            if ((it->ip == rip) && (it->port == rport)) {
                if (it == tail) {
                    tail = prev ;
                }
                prev->next = it->next ;
                return 0;
            }
            prev = it ;
            it = it->next ;
        }
        return 0 ;
    }
    ~ClientList() {}
protected:

};


#endif /* ClientList_hpp */
