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

void VertexBuffer::fillData(float *vertices, unsigned int count)
{
    bind();
    GLCall( glBufferData(GL_ARRAY_BUFFER,count * sizeof(float),vertices,GL_STATIC_DRAW) );
    
}