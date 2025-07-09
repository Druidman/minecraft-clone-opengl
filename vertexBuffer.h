#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H
#include <vector>
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"

class VertexBuffer{
    private:
        unsigned int m_vbo;
    public:
        VertexBuffer();
        void bind();

        template <typename T>
        void fillData(const std::vector< T > &vertices);
};


template <typename T>
inline void VertexBuffer::fillData(const std::vector< T > &vertices){
    bind();
    GLCall( glBufferData(GL_ARRAY_BUFFER,vertices.size() * sizeof(T),vertices.data(),GL_DYNAMIC_DRAW) );
}
#endif