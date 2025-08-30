#include "gpuBuffer.h"
#include "vendor/glad/glad.h"

bool GpuBuffer::uploadData(const void *data, BufferInt size, BufferInt start)
{
    if (
        size == 0 ||
        start + size > this->bufferSize
    ){
        return false;
    }
    bind();
    GLCall( glBufferSubData(bufferType, start, size, data) );
    return true;
    
}

bool GpuBuffer::allocateBuffer(BufferInt size)
{
    if (size == 0){
        return false;
    }
    bind();
    GLCall( glBufferData(bufferType, size, nullptr, GL_DYNAMIC_DRAW) );
    this->bufferSize = size;
    return true;
}

bool GpuBuffer::expandBuffer(BufferInt by)
{
    std::cout << "\n\nBUFFER EXPANSION..\n\n";
    std::cout << by << "\n" << this->bufferSize;
    BufferInt newBufferSize = this->bufferSize + by;
    newBufferSize -= newBufferSize % 4; // for fitting // TODO

    // create temp buffer
    GpuBuffer tempBuffer = GpuBuffer(bufferType);
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

    GLCall( glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, tempBuffer.bufferSize) ); 

    tempBuffer.unBind();
    this->unBind();
    std::cout << "End of expansion\n";
 
    return true;
}

bool GpuBuffer::moveBufferPart()
{
    return false;
}

GpuBuffer::GpuBuffer(GLenum bufferType) : Buffer()
{
    GLCall( glGenBuffers(1,&m_bo) );
    this->bufferType = bufferType;
    bind();
}

GpuBuffer::~GpuBuffer()
{
    if (m_bo != 0) {
        GLCall( glDeleteBuffers(1,&m_bo) ); 
        std::cout << "delete Buffer \n";
    }
    
}

void GpuBuffer::bind()
{
    GLCall( glBindBuffer(bufferType,m_bo) );
    lastBoundTarget = this->bufferType;
}

void GpuBuffer::unBind()
{
    GLCall( glBindBuffer(bufferType,0) );
}

void GpuBuffer::bindAsRead(){
    GLCall( glBindBuffer(GL_COPY_READ_BUFFER,m_bo) );
    lastBoundTarget = GL_COPY_READ_BUFFER;
}
void GpuBuffer::bindAsWrite(){
    GLCall( glBindBuffer(GL_COPY_WRITE_BUFFER,m_bo) );
    lastBoundTarget = GL_COPY_WRITE_BUFFER;
}

BufferInt GpuBuffer::getBufferSize()
{
    return bufferSize;
}






