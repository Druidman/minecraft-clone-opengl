#ifndef STORAGEBUFFER_H
#define STORAGEBUFFER_H

#include <vector>


#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"

#include "betterGL.h"
#include "chunk.h"
#include "buffer.h"
#include "world.h"



class StorageBuffer : public Buffer {
    private:
        World *world;

        // this is relatively small data so we keep it here for optimized buffer insertions
        std::vector<StorageBufferType> bufferContent;
    public:
        StorageBuffer() : Buffer(GL_SHADER_STORAGE_BUFFER){};
        void init(World *world);
        void setBindingPoint(int port);

        bool fillBufferWithChunks(std::vector<Chunk*> *chunks);
        
};
#endif