#ifndef BUFFER_H
#define BUFFER_H

#include "betterGL.h"
#include <vector>
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"
#include <utility>

typedef unsigned long long int BufferInt;
typedef glm::vec4 StorageBufferType;

enum BufferType {
    STORAGE_BUFFER, MESH_BUFFER, INDIRECT_BUFFER, NONE
};

struct DrawArraysIndirectCommand{
    unsigned int  count;
    unsigned int  instanceCount;
    unsigned int  first;
    unsigned int  baseInstance;
};
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
        bool fillData(const std::vector< T > *data);

        template <typename T>
        bool fillData(const T data);

        template <typename T>
        bool updateData(const std::vector< T > *data, BufferInt dataByteStart, BufferInt dataByteEnd);
        template <typename T>
        bool updateData(const T data,BufferInt dataByteStart, BufferInt dataByteEnd);
};
template <typename T>
bool Buffer::fillData(const std::vector< T > *data){
    bind();
    GLCall( glBufferData(bufferType, sizeof(T) * data->size(), data->data(), GL_STATIC_DRAW) );
    this->bufferSize = sizeof(T) * data->size();
    return true;
}

template <typename T>
bool Buffer::fillData(const T data){
    bind();
    GLCall( glBufferData(bufferType, sizeof(T), &data, GL_STATIC_DRAW) );
    this->bufferSize = sizeof(T);
    return true;
}

template <typename T>
bool Buffer::updateData(const std::vector< T > *data, BufferInt dataByteStart, BufferInt dataByteEnd){
    BufferInt dataSize = data->size() * sizeof(T);
    if (
        dataSize == 0 ||
        dataByteStart == dataByteEnd ||
        dataByteEnd > this->bufferSize || 
        dataByteStart + dataSize > dataByteEnd
    ){
        return false;
    }
    bind();
    GLCall( glBufferSubData(bufferType, dataByteStart, dataSize, data->data()) );
    return true;
}

template <typename T>
bool Buffer::updateData(const T data, BufferInt dataByteStart, BufferInt dataByteEnd){
    BufferInt dataSize = sizeof(T);
    if (
        dataSize == 0 ||
        dataByteStart == dataByteEnd ||
        dataByteEnd > this->bufferSize || 
        dataByteStart + dataSize > dataByteEnd
    ){
        return false;
    }
    bind();
    GLCall( glBufferSubData(bufferType, dataByteStart, dataSize, &data) );
    return true;
}



#endif
