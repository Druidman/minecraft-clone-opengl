#ifndef ID_BUFFER_H
#define ID_BUFFER_H
#include <vector>
#include "buffer.h"
#include "chunk.h"

class IdBuffer : public Buffer{
    private:
        // this is relatively small data so we keep it here for optimized buffer insertions
        std::vector<int> bufferContent;
        
    public:
        IdBuffer() : Buffer(GL_ARRAY_BUFFER){};
    

        bool fillBufferWithChunks(std::vector<Chunk*> *chunks, size_t elements);
};

#endif