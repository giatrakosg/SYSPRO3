//
//  CircularBuffer.cpp
//  SYSPRO3
//
//  Created by <author> on 31/05/2019.
//
//

#include "CircularBuffer.hpp"
CBNode::CBNode(long ip,short port,char *path,char *ver) : ip(ip) , port(port) {
    if (path != NULL) {
        strcpy(pathname,path);
    }
    if (ver != NULL) {
        strcpy(version,ver);
    }
}

CircularBuffer::CircularBuffer(int max_length) : head(0) , tail(0) , length(max_length) , curr(0) {
    data = new CBNode*[length];
    pthread_cond_init(&isempty, NULL);
    pthread_cond_init(&isfull, NULL);
    mtx = PTHREAD_MUTEX_INITIALIZER ;

}
int CircularBuffer::put(long ip,short port,char *path,char *version) {
    int err ;
    CBNode *node = new CBNode(ip,port,path,version) ;
    if (err = pthread_mutex_lock(&mtx)) { /* Lock mutex */
        perror2("pthread_mutex_lock", err); exit(1); }

    // If the buffer is full , then wait the condition
    while (curr == length) {
        pthread_cond_wait(&isfull, &mtx); /* Wait for signal */
    }
    data[head] = node ;
    head = (head + 1) % length ;
    curr++;

    pthread_cond_signal(&isempty); /* Awake other thread */
    if (err = pthread_mutex_unlock(&mtx)) { /* Unlock mutex */
        perror2("pthread_mutex_unlock", err); exit(1); }

    return 0 ;

}
int CircularBuffer::get(long &outip,short &outport,char *&outpath,char *&outver) {
    int err ;
    if (err = pthread_mutex_lock(&mtx)) { /* Lock mutex */
        perror2("pthread_mutex_lock", err); exit(1); }
    while (curr == 0) {
        pthread_cond_wait(&isempty, &mtx); /* Wait for signal */
    }
        outip = data[tail]->ip ;
        outport = data[tail]->port ;
        outpath = new char[512];
        strcpy(outpath,data[tail]->pathname);
        outver = new char[32];
        strcpy(outver,data[tail]->version);
        curr--;
        delete data[tail];
        data[tail] = NULL ;
        tail = (tail + 1) % length ;
    pthread_cond_signal(&isfull); /* Awake other thread */
    if (err = pthread_mutex_unlock(&mtx)) { /* Unlock mutex */
        perror2("pthread_mutex_unlock", err); exit(1); }

    return 0 ;
}
CircularBuffer::~CircularBuffer() {}
