#ifndef BUFFER_H
#define BUFFER_H

#include "vendor/glm/glm.hpp"

typedef glm::vec4 StorageBufferType;
typedef unsigned long long int BufferInt;
struct DrawArraysIndirectCommand{
    unsigned int  count;
    unsigned int  instanceCount;
    unsigned int  first;
    unsigned int  baseInstance;
};
class Buffer{
    public:
        BufferInt bufferSize = 0;
    protected:
        Buffer(){};
    public:
        virtual bool uploadData(const void* data, BufferInt size, BufferInt start) =0 ;
        virtual bool allocateBuffer(BufferInt size) = 0;
        virtual bool expandBuffer(BufferInt by) = 0;
        virtual bool moveBufferPart() = 0;

        virtual void* getBufferContent(){
            return nullptr;
        };
    
};
#endif