#ifndef CPUBUFFER_H
#define CPUBUFFER_H

#include "buffer.h"
#include <cstring>


template <typename ELEMENT_TYPE>
class CpuBuffer : public Buffer{
    protected:
        std::vector<ELEMENT_TYPE> bufferContent;
    public:
        ELEMENT_TYPE PLACE_HOLDER_ELEMENT;

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


template <typename ELEMENT_TYPE>
void* CpuBuffer<ELEMENT_TYPE>::getBufferContent(){
    return this->bufferContent.data();
};


template <typename ELEMENT_TYPE>
inline bool CpuBuffer<ELEMENT_TYPE>::uploadData(const void *data, BufferInt size, BufferInt start)
{
    if (
        size == 0 ||
        start == start + size ||
        start + size > this->bufferSize
    ){
        return false;
    }
    
    std::memcpy(&this->bufferContent[start / sizeof(ELEMENT_TYPE)], data, size);
    
    return true;
}

template <typename ELEMENT_TYPE>
inline bool CpuBuffer<ELEMENT_TYPE>::allocateBuffer(BufferInt size)
{
    if (size == 0){
        return false;
    }
    this->bufferContent.clear();
    this->bufferContent.resize(size / sizeof(ELEMENT_TYPE), PLACE_HOLDER_ELEMENT);
    this->bufferSize = size;
    return true;
}

template <typename ELEMENT_TYPE>
inline bool CpuBuffer<ELEMENT_TYPE>::expandBuffer(BufferInt by)
{
    if (by == 0){
        return false;
    }
    this->bufferContent.resize((this->bufferSize + by) / sizeof(ELEMENT_TYPE), PLACE_HOLDER_ELEMENT);
    this->bufferSize += by;
    return true;
}

template <typename ELEMENT_TYPE>
inline bool CpuBuffer<ELEMENT_TYPE>::moveBufferPart()
{
    return false;
}

#endif
