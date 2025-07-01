#ifndef ELEMENT_BUFFER_H
#define ELEMENT_BUFFER_H

class ElementBuffer{
    private:
        unsigned int m_ebo;
    public:
        ElementBuffer();
        void bind();
        void fillData(int *indicies, unsigned int count);
};
#endif