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



