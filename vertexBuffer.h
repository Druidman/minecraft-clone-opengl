#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#ifdef __EMSCRIPTEN__
    #include <GLES3/gl3.h>
#else
    #include <GL/glew.h>
#endif

#include <vector>
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"

class VertexBuffer{
    private:

        unsigned int m_vbo;
        
        unsigned long long int dataFilled = 0;
        unsigned long long int bufferSize = 0;
  
    public:
        
        VertexBuffer();
        void bind();
        void unBind();
        unsigned long long getBufferSize(){ return bufferSize; };

        void allocateBuffer(unsigned long long int size);

        template <typename T>
        void fillData(const std::vector< T > &data);

        template <typename T>
        bool addData(const std::vector< T > &data);
};


template <typename T>
inline void VertexBuffer::fillData(const std::vector< T > &data){
    // allocates new memory
    bind();
    allocateBuffer(data.size() * sizeof(T));
    addData(data);
    
}

template <typename T>
bool VertexBuffer::addData(const std::vector< T > &data){
    bind();
    if (this->bufferSize == 0){
        return false;
    }
    if (this->dataFilled == this->bufferSize){
        return false;
        
    }
    GLCall( glBufferSubData(GL_ARRAY_BUFFER, this->dataFilled, sizeof(T) * data.size(), data.data()) );
    this->dataFilled += sizeof(T) * data.size();
    return true;

}



#endif