#ifndef MESHBUFFER_H
#define MESHBUFFER_H

#include <vector>
#include <utility>

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"

#include "dynamicBuffer.h"
#include "betterGL.h"
#include "chunk.h"
#include "buffer.h"

class MeshBuffer : public DynamicBuffer {
    protected:
        
        virtual BufferInt getChunkDataSize(Chunk* chunk) override;
        virtual bool requiresContiguousMemoryLayout() override;
    public:
        MeshBuffer() : DynamicBuffer(GL_ARRAY_BUFFER){
            BUFFER_PADDING = 20; // expressed in %
            CHUNK_PADDING = 10;
        };
    
        virtual bool updateChunkBuffer(Chunk* chunk) override;
        virtual bool insertChunksToBuffer(std::vector<Chunk*> *chunks) override;
        
};
#endif