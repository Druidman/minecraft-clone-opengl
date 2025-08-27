#ifndef ID_BUFFER_H
#define ID_BUFFER_H
#include <vector>

#include "gpuBuffer.h"
#include "chunk.h"
#include "buffer.h"


class IdBuffer : public GpuBuffer{
    private:
        // this is relatively small data so we keep it here for optimized buffer insertions
        
        
    public:
        std::vector<int> bufferContent;
        IdBuffer() : GpuBuffer(GL_ARRAY_BUFFER){};
    

        bool fillBufferWithChunks(std::vector<Chunk*> *chunks, size_t elements);
};

#endif