#ifndef ELEMENT_BUFFER_H
#define ELEMENT_BUFFER_H
#include <vector>

class ElementBuffer{
    private:
        unsigned int m_ebo;
    public:
        ElementBuffer();
        void bind();
        template <typename T>
        void fillData(const std::vector< T > &indices);
};
template <typename T>
inline void ElementBuffer::fillData(const std::vector< T > &indices){
    bind();
    GLCall( glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(T) * indices.size(), indices.data(), GL_STATIC_DRAW) );
}
#endif