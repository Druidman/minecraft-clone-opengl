#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H


class VertexArray{
    private:
        unsigned int m_vao;
    public:
        VertexArray();
        void bind();
        void setAttr(GLuint index, int count, GLenum type, GLsizei stride, size_t offset);
        void setAttrI(GLuint index, int count, GLsizei stride, size_t offset);
};
#endif