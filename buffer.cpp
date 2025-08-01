#include "buffer.h"
#include "vendor/glad/glad.h"

Buffer::Buffer(GLenum bufferType)
{
    GLCall( glGenBuffers(1,&m_bo) );
    this->bufferType = bufferType;
    bind();

}

void Buffer::bind()
{
    GLCall( glBindBuffer(this->bufferType,m_bo) );
}

void Buffer::unBind()
{
    GLCall( glBindBuffer(this->bufferType,0) );
}

void Buffer::allocateBuffer(unsigned long long int size)
{
    bind();
    GLCall( glBufferData(this->bufferType, size, NULL, GL_STATIC_DRAW) );
    this->bufferSize = size;
    this->dataFilled = 0;
}
