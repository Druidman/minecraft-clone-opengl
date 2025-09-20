#include "idBuffer.h"

void IdBuffer::postChunkUpdateFunction()
{
    gpuBufferRequiresRefill = true;
 
}

bool IdBuffer::fillGpuBuffer()
{
    if (!gpuBufferRequiresRefill){
        return true;
    }
    if (gpuBuffer.bufferSize != this->cpuBuffer.bufferSize){
        if (!gpuBuffer.allocateBuffer(this->cpuBuffer.bufferSize)){
            return false;
        };
    }

    if (!gpuBuffer.uploadData(this->cpuBuffer.getBufferContent(), this->cpuBuffer.bufferSize, 0)){
        return false;
    };
 
    std::cout << "ID_GPU_BUFFER_CALL\n";
    gpuBufferRequiresRefill = false;
    return true;

}
