#include "betterGL.h"

#include "vertexBuffer.h"


VertexBuffer::VertexBuffer()
{
    GLCall( glGenBuffers(1,&m_vbo) );
    bind();

}

void VertexBuffer::bind()
{
    GLCall( glBindBuffer(GL_ARRAY_BUFFER,m_vbo) );
}

void VertexBuffer::unBind()
{
    GLCall( glBindBuffer(GL_ARRAY_BUFFER,0) );
}

void VertexBuffer::allocateBuffer(unsigned long long int size)
{
    bind();
    GLCall( glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW) );
    this->bufferSize = size;
    this->dataFilled = 0;
}
