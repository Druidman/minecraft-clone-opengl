#include "betterGL.h"

#include "elementBuffer.h"

ElementBuffer::ElementBuffer()
{
    GLCall( glGenBuffers(1,&m_ebo) );
    bind();

}

void ElementBuffer::bind()
{
    GLCall( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ebo) );
}

void ElementBuffer::fillData(int *indicies, unsigned int count)
{
    bind();
    GLCall( glBufferData(GL_ELEMENT_ARRAY_BUFFER,count * sizeof(int) , indicies, GL_STATIC_DRAW) );
    
}