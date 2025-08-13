#ifndef STORAGEBUFFER_H
#define STORAGEBUFFER_H

#include <vector>
#include <utility>

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"

#include "dynamicBuffer.h"
#include "betterGL.h"
#include "chunk.h"
#include "buffer.h"
#include "world.h"


class StorageBuffer : public DynamicBuffer {
    private:
        World *world;

    protected:
        virtual BufferInt getChunkDataSize(Chunk* chunk) override;
        virtual bool requiresContiguousMemoryLayout() override;
    public:
        StorageBuffer() : DynamicBuffer(GL_SHADER_STORAGE_BUFFER){};
        void init(World *world);
        void setBindingPoint(int port);
    
        virtual bool updateChunkBuffer(Chunk* chunk) override;
        virtual bool insertChunksToBuffer(std::vector<Chunk*> *chunks) override;
        
};
#endif