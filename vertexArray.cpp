#include "betterGL.h"

#include "vertexArray.h"


VertexArray::VertexArray()
{
    GLCall( glGenVertexArrays(1,&m_vao) );
    bind();

}

void VertexArray::bind()
{
    GLCall( glBindVertexArray(m_vao) );
}

void VertexArray::setAttr(GLuint index, int count, GLenum type, GLsizei stride, size_t offset)
{
    bind();
    GLCall( glEnableVertexAttribArray(index) );
    GLCall( glVertexAttribPointer(index, count, type, GL_FALSE, stride, (void*)offset) );
    
}
void VertexArray::setAttrI(GLuint index, int count, GLsizei stride, size_t offset)
{
    bind();
    GLCall( glEnableVertexAttribArray(index) );
    GLCall( glVertexAttribIPointer(index, count, GL_INT, stride, (void*)offset) );
    
}
