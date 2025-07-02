#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

class VertexBuffer{
    private:
        unsigned int m_vbo;
    public:
        VertexBuffer();
        void bind();

        template <typename T>
        void fillData(T *vertices, unsigned int count);
};

template <typename T>
inline void VertexBuffer::fillData(T *vertices, unsigned int count)
{
    bind();
    GLCall( glBufferData(GL_ARRAY_BUFFER,count * sizeof(T),vertices,GL_STATIC_DRAW) );
    
}
#endif