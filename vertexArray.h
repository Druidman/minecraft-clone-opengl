#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H
#include <GL/glew.h>

class VertexArray{
    private:
        unsigned int m_vao;
    public:
        VertexArray();
        void bind();
        void setAttr(GLuint index, int count, GLenum type, GLsizei stride, size_t offset);
};
#endif