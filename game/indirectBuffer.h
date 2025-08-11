#ifndef INDIRECTBUFFER_H
#define INDIRECTBUFFER_H

#include <vector>
#include <utility>

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"

#include "dynamicBuffer.h"
#include "betterGL.h"
#include "chunk.h"
#include "buffer.h"

class IndirectBuffer : public DynamicBuffer {
    protected:
        virtual BufferInt getChunkDataSize(Chunk* chunk) override;
    public:
        IndirectBuffer() : DynamicBuffer(GL_DRAW_INDIRECT_BUFFER){};
    
        virtual bool updateChunkBuffer(Chunk* chunk) override;
        
};
#endif