#ifndef BUFFER_H
#define BUFFER_H

#include "betterGL.h"
#include <vector>
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"

class Buffer{
    private:
        GLenum bufferType;
        unsigned int m_bo;
        
        unsigned long long int dataFilled = 0;
        unsigned long long int bufferSize = 0;
        unsigned long long int dataCount = 0;
  
    public:
        
        Buffer(GLenum bufferType);
        void bind();
        void unBind();
        unsigned int getId(){return m_bo;};
        unsigned long long getFilledDataSize(){ return dataFilled; };
        unsigned long long getFilledDataCount(){ return dataCount; };
        unsigned long long getBufferSize(){ return bufferSize; };

        void allocateBuffer(unsigned long long int size);

        template <typename T>
        void fillData(const std::vector< T > *data);
        template <typename T>
        void fillData(const std::vector< T > *data, size_t sizeToAlloc);

        template <typename T>
        bool addData(const std::vector< T > *data);
        template <typename T>
        bool addData(const T data);

        template <typename T>
        bool updateData(const std::vector< T > *data, unsigned long long int dataByteOffset);
        template <typename T>
        bool updateData(const T data, unsigned long long int dataByteOffset);
};


template <typename T>
inline void Buffer::fillData(const std::vector< T > *data){
    // allocates new memory
    bind();
    allocateBuffer(data->size() * sizeof(T));
    addData<T>(data);
}

template <typename T>
inline void Buffer::fillData(const std::vector< T > *data, size_t sizeToAlloc){
    // allocates new memory
    bind();
    allocateBuffer(sizeToAlloc);
    addData<T>(data);
}

template <typename T>
bool Buffer::addData(const std::vector< T > *data){
    bind();
    if (data->size() == 0){
        return false;
    }
    if (this->bufferSize == 0){
        return false;   
    }
    if (this->dataFilled == this->bufferSize){
        return false;   
    }
    if (data->size() * sizeof(T) + dataFilled > this->bufferSize){
        ExitError("BUFFER","too much data");
        return false;
    }
    GLCall( glBufferSubData(bufferType, (size_t)this->dataFilled, (size_t)sizeof(T) * (size_t)data->size(), data->data()) );
    this->dataFilled += sizeof(T) * data->size();
    this->dataCount += data->size();
    return true;

}

template <typename T>
bool Buffer::addData(const T data){
    bind();
    if (this->bufferSize == 0){
        return false;
    }
    if (this->dataFilled == this->bufferSize){
        return false;
        
    }
    GLCall( glBufferSubData(bufferType, (size_t)this->dataFilled, (size_t)sizeof(T), &data) );
    this->dataFilled += sizeof(T);
    this->dataCount += 1;
    return true;

}
template <typename T>
bool Buffer::updateData(const std::vector< T > *data, unsigned long long int dataByteOffset){
    bind();

    unsigned long long dataByteSize = data->size() * sizeof(T);
    if (dataByteOffset + dataByteSize > this->bufferSize){
        return false;
    }
    GLCall( glBufferSubData(bufferType, dataByteOffset, dataByteSize, data->data()) );
    this->dataFilled += (dataByteOffset + dataByteSize) - dataFilled; //calc if update filled more data then there already was
    return true;
}

template <typename T>
bool Buffer::updateData(const T data, unsigned long long int dataByteOffset){
    bind();
    
    unsigned long long dataByteSize = sizeof(T);
    if (dataByteOffset + dataByteSize > this->bufferSize){
        return false;
    }
    GLCall( glBufferSubData(bufferType, dataByteOffset, dataByteSize, &data) );
    this->dataFilled += (dataByteOffset + dataByteSize) - dataFilled; //calc if update filled more data then there already was
    return true;
}



#endif