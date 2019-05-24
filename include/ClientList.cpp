//
//  ClientList.cpp
//  SYSPRO3
//
//  Created by <author> on 22/05/2019.
//
//

#include "ClientList.hpp"

ClientList::ClientList() : size(0) , head(NULL) , tail(head) {}
void ClientList::addNode(long nip,short nport) {
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
void ClientList::print(void) {
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
ClientList::~ClientList() {}
