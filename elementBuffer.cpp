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