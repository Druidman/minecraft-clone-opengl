#ifndef BUFFER_H
#define BUFFER_H

#include "betterGL.h"
#include <vector>
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"
#include <utility>

typedef unsigned long long int BufferInt;
class Buffer{
    protected:
        GLenum bufferType;
        GLenum lastBoundTarget;
        unsigned int m_bo;
        BufferInt bufferSize = 0;
    public:
        
        Buffer(GLenum bufferType);
        ~Buffer();
        void bind();
        void unBind();
        void bindAsRead();
        void bindAsWrite();
        unsigned int getId(){return m_bo;};
        BufferInt getBufferSize(){ return bufferSize; };

        void allocateBuffer(BufferInt size);
        void expandBuffer(BufferInt by);

        template <typename T>
        bool updateData(const std::vector< T > *data, BufferInt dataByteStart, BufferInt dataByteEnd);
        template <typename T>
        bool updateData(const T data,BufferInt dataByteStart, BufferInt dataByteEnd);
};

template <typename T>
bool Buffer::updateData(const std::vector< T > *data, BufferInt dataByteStart, BufferInt dataByteEnd){
    BufferInt dataSize = data->size() * sizeof(T);
    if (
        dataSize == 0 ||
        dataByteStart == dataByteEnd ||
        dataByteEnd > this->bufferSize || 
        dataByteStart + dataSize > dataByteEnd ||
    ){
        return false;
    }
    bind();
    GLCall( glBufferSubData(bufferType, dataByteStart, dataByteSize, data->data()) );
    return true;
}

template <typename T>
bool Buffer::updateData(const T data, BufferInt dataByteStart, BufferInt dataByteEnd){
    BufferInt dataSize = sizeof(T);
    if (
        dataSize == 0 ||
        dataByteStart == dataByteEnd ||
        dataByteEnd > this->bufferSize || 
        dataByteStart + dataSize > dataByteEnd ||
    ){
        return false;
    }
    bind();
    GLCall( glBufferSubData(bufferType, dataByteStart, dataByteSize, &data) );
    return true;
}



#endif
