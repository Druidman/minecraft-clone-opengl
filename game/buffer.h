#ifndef BUFFER_H
#define BUFFER_H

#include "betterGL.h"
#include <vector>
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"
#include <utility>

typedef unsigned long long int BufferInt;
class Buffer{
    private:
        GLenum bufferType;
        unsigned int m_bo;
        BufferInt bufferSize = 0;
    public:
        
        Buffer(GLenum bufferType);
        void bind();
        void unBind();
        unsigned int getId(){return m_bo;};
        BufferInt getBufferSize(){ return bufferSize; };

        void allocateBuffer(BufferInt size);

        template <typename T>
        bool updateData(const std::vector< T > *data, BufferInt dataByteOffset);
        template <typename T>
        bool updateData(const T data, BufferInt dataByteOffset);
};

template <typename T>
bool Buffer::updateData(const std::vector< T > *data, BufferInt dataByteOffset){
    BufferInt dataSize = data->size() * sizeof(T);
    if (
        dataByteOffset < 0 ||
        dataSize == 0 ||
        dataByteOffset + dataSize > this->bufferSize
    ){
        return false;
    }
    bind();
    GLCall( glBufferSubData(bufferType, dataByteOffset, dataByteSize, data->data()) );
    return true;
}

template <typename T>
bool Buffer::updateData(const T data, BufferInt dataByteOffset){
    BufferInt dataSize = sizeof(T);
    if (
        dataByteOffset < 0 ||
        dataSize == 0 ||
        dataByteOffset + dataSize > this->bufferSize
    ){
        return false;
    }
    bind();
    GLCall( glBufferSubData(bufferType, dataByteOffset, dataByteSize, &data) );
    return true;
}



#endif