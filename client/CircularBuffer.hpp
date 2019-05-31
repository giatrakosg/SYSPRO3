//
//  CircularBuffer.hpp
//  SYSPRO3
//
//  Created by <author> on 31/05/2019.
//
//

#ifndef CircularBuffer_hpp
#define CircularBuffer_hpp

#include <stdio.h>
#include <pthread.h>


struct CBNode {
    long  ip            ;
    short port          ;
    char  pathname[512] ;
    char  version[32]   ;
};


class CircularBuffer {
private:
    int             head    ;
    int             tail    ;
    pthread_cond_t  isempty ;
    pthread_cond_t  isfull  ;
    int             length  ;
    int             cap     ;
    CBNode *        data    ;
    pthread_mutex_t mtx     ;
public:
    CircularBuffer(int );
    int put(long ,short ,char *,char *);
    int get(long &,short &,char *&,char *&);
    ~CircularBuffer();
protected:

};


#endif /* CircularBuffer_hpp */
