#ifndef CPUBUFFER_H
#define CPUBUFFER_H

#include "buffer.h"
#include <cstring>


template <typename ELEMENT_TYPE>
class CpuBuffer : public Buffer{
    protected:
        std::vector<ELEMENT_TYPE> bufferContent;

    public:
        virtual bool uploadData(const void* data, BufferInt size, BufferInt start) override;
        virtual bool allocateBuffer(BufferInt size) override;
        virtual bool expandBuffer(BufferInt by) override ;
        virtual bool moveBufferPart() override;

        virtual void* getBufferContent() override;
    
    public:
        CpuBuffer() : Buffer(){
            
        }

};


template <typename T>
void* CpuBuffer<T>::getBufferContent(){
    return &this->bufferContent;
};


template <typename T>
inline bool CpuBuffer<T>::uploadData(const void *data, BufferInt size, BufferInt start)
{
    if (
        size == 0 ||
        start == start + size ||
        start + size > this->bufferSize
    ){
        return false;
    }
    std::memcpy(&this->bufferContent[start / sizeof(T)], data, size);
    
    return true;
}

template <typename T>
inline bool CpuBuffer<T>::allocateBuffer(BufferInt size)
{
    this->bufferContent.clear();
    this->bufferContent.resize(size / sizeof(T));
    this->bufferSize = size;
    return true;
}

template <typename T>
inline bool CpuBuffer<T>::expandBuffer(BufferInt by)
{
    this->bufferContent.resize((this->bufferSize + by) / sizeof(T));
    return true;
}

template <typename T>
inline bool CpuBuffer<T>::moveBufferPart()
{
    return false;
}

#endif
