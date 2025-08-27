#ifndef INDIRECTBUFFER_H
#define INDIRECTBUFFER_H

#include <vector>
#include <utility>
#include <string>


#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"

#include "dynamicBuffer.h"
#include "betterGL.h"
#include "chunk.h"
#include "buffer.h"

typedef unsigned int uint;

class IndirectBuffer : public GpuBuffer {
    private:
        // this is relatively small data so we keep it here for optimized buffer insertions
        std::vector<DrawArraysIndirectCommand> bufferContent;
        
    public:
        IndirectBuffer() : GpuBuffer(GL_DRAW_INDIRECT_BUFFER){};
    

        bool fillBufferWithChunks(std::vector<Chunk*> *chunks);
        
};
#endif