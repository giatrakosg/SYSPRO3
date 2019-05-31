//
//  CircularBuffer.cpp
//  SYSPRO3
//
//  Created by <author> on 31/05/2019.
//
//

#include "CircularBuffer.hpp"

CircularBuffer::CircularBuffer(int max_length) : head(0) , tail(0) , isempty(true) , isfull(false) , cap(0) , length(max_length) {
    data = new CBNode[length];
    pthread_cond_init(&isempty, NULL);
    pthread_cond_init(&isfull, NULL);
    mtx = PTHREAD_MUTEX_INITIALIZER ;

}
CircularBuffer::~CircularBuffer() {}
