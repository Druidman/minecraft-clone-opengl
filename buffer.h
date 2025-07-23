#ifndef BUFFER_H
#define BUFFER_H

#ifdef __EMSCRIPTEN__
    #include <GLES3/gl3.h>
#else
    #include <GL/glew.h>
#endif

#include <vector>
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"

class Buffer{
    private:
        GLenum bufferType;
        unsigned int m_bo;
        
        unsigned long long int dataFilled = 0;
        unsigned long long int bufferSize = 0;
  
    public:
        
        Buffer(GLenum bufferType);
        void bind();
        void unBind();
        unsigned int getId(){return m_bo;};
        unsigned long long getFilledDataSize(){ return dataFilled; };
        unsigned long long getBufferSize(){ return bufferSize; };

        void allocateBuffer(unsigned long long int size);

        template <typename T>
        void fillData(const std::vector< T > &data);

        template <typename T>
        bool addData(const std::vector< T > &data);

        template <typename T>
        bool addData(const T data);
};


template <typename T>
inline void Buffer::fillData(const std::vector< T > &data){
    // allocates new memory
    bind();
    allocateBuffer(data.size() * sizeof(T));
    addData<T>(data);
    
}

template <typename T>
bool Buffer::addData(const std::vector< T > &data){
    bind();
    if (this->bufferSize == 0){
        return false;
    }
    if (this->dataFilled == this->bufferSize){
        return false;
        
    }
    GLCall( glBufferSubData(bufferType, this->dataFilled, sizeof(T) * data.size(), data.data()) );
    this->dataFilled += sizeof(T) * data.size();
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
    GLCall( glBufferSubData(bufferType, this->dataFilled, sizeof(T), &data) );
    this->dataFilled += sizeof(T);
    return true;

}



#endif