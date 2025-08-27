#ifndef GPUBUFFER_H
#define GPUBUFFER_H

#include "betterGL.h"
#include "buffer.h"
#include <vector>
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"
#include <utility>







class GpuBuffer : public Buffer{
    protected:
        GLenum bufferType;
        GLenum lastBoundTarget;
        unsigned int m_bo;
        
    public:
        virtual bool uploadData(const void* data, BufferInt size, BufferInt start) override;
        virtual bool allocateBuffer(BufferInt size) override;
        virtual bool expandBuffer(BufferInt by) override ;
        virtual bool moveBufferPart() override;
    public:
        
        GpuBuffer(GLenum bufferType);
        ~GpuBuffer();
        void bind();
        void unBind();
        void bindAsRead();
        void bindAsWrite();
        unsigned int getId(){return m_bo;};
        BufferInt getBufferSize();
        
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
bool GpuBuffer::fillData(const std::vector< T > *data){
    bind();
    GLCall( glBufferData(bufferType, sizeof(T) * data->size(), data->data(), GL_DYNAMIC_DRAW) );
    this->bufferSize = sizeof(T) * data->size();
    return true;
}

template <typename T>
bool GpuBuffer::fillData(const T data){
    bind();
    GLCall( glBufferData(bufferType, sizeof(T), &data, GL_DYNAMIC_DRAW) );
    this->bufferSize = sizeof(T);
    return true;
}

template <typename T>
bool GpuBuffer::updateData(const std::vector< T > *data, BufferInt dataByteStart, BufferInt dataByteEnd){
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
bool GpuBuffer::updateData(const T data, BufferInt dataByteStart, BufferInt dataByteEnd){
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
