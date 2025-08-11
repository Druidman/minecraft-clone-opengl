#include "buffer.h"
#include "vendor/glad/glad.h"

Buffer::Buffer(GLenum bufferType)
{
    GLCall( glGenBuffers(1,&m_bo) );
    this->bufferType = bufferType;
    bind();
}

Buffer::~Buffer()
{
    if (m_bo != 0) {
        GLCall( glDeleteBuffers(1,&m_bo) ); 
        std::cout << "delete Buffer \n";
    }
    
}

void Buffer::bind()
{
    GLCall( glBindBuffer(bufferType,m_bo) );
    lastBoundTarget = this->bufferType;
}

void Buffer::unBind()
{
    GLCall( glBindBuffer(bufferType,0) );
}

void Buffer::bindAsRead(){
    GLCall( glBindBuffer(GL_COPY_READ_BUFFER,m_bo) );
    lastBoundTarget = GL_COPY_READ_BUFFER;
}
void Buffer::bindAsWrite(){
    GLCall( glBindBuffer(GL_COPY_WRITE_BUFFER,m_bo) );
    lastBoundTarget = GL_COPY_WRITE_BUFFER;
}

void Buffer::allocateBuffer(unsigned long long int size)
{
    std::cout << "allocating: " << size << " in buffer: "<< m_bo << "\n";
    bind();
    GLCall( glBufferData(bufferType, size, nullptr, GL_STATIC_DRAW) );
    this->bufferSize = size;
}

void Buffer::expandBuffer(BufferInt by){
    BufferInt newBufferSize = this->bufferSize + by;

    // create temp buffer
    Buffer tempBuffer = Buffer(bufferType);
    tempBuffer.allocateBuffer(this->bufferSize);

    // setup for copying
    tempBuffer.bindAsWrite();
    this->bindAsRead();

    // copy data to temp buffer
    GLCall( glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, this->bufferSize) ); 

    //expand buffer
    this->allocateBuffer(newBufferSize); 

    // setup for copying
    tempBuffer.bindAsRead();
    this->bindAsWrite();

    GLCall( glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, this->bufferSize) ); 

    tempBuffer.unBind();
    this->unBind();
}
