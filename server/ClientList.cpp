//
//  ClientList.cpp
//  SYSPRO3
//
//  Created by <author> on 22/05/2019.
//
//

#include "ClientList.hpp"

ClientList::ClientList() : size(0) , head(NULL) , tail(head) {}
void ClientList::addNode(struct sockaddr_in inp) {
    if (head == NULL) {
        struct Node *node = new Node ;
        node->data = inp ;
        node->next = NULL ;
        head = node ;
        tail = head ;
        size++;
        return ;
    } else {
        struct Node *node = new Node ;
        node->data = inp ;
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
        char *clientip = new char[20];
        strcpy(clientip, inet_ntoa(ind->data.sin_addr));
        printf("Client %d <%s:%d> \n",client, clientip,ind->data.sin_port);
        client++;
        ind = ind->next ;
    }
    printf("------------------\n" );

}
ClientList::~ClientList() {}
