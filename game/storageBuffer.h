#ifndef STORAGEBUFFER_H
#define STORAGEBUFFER_H

#include <vector>


#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"

#include "betterGL.h"
#include "chunk.h"
#include "buffer.h"
#include "world.h"


const int UNIFORM_BUFFER_LENGTH = 1024;
class StorageBuffer : public Buffer {
    private:

        World *world;

        // this is relatively small data so we keep it here for optimized buffer insertions
        std::vector<StorageBufferType> bufferContent;
    public:
        StorageBuffer(GLenum bufferType) : Buffer(bufferType){
            if (bufferType != GL_SHADER_STORAGE_BUFFER && bufferType != GL_UNIFORM_BUFFER ){
                ExitError("STORAGE_BUFFER","Can't create storage buffer different than uniform/shaderStorage buffer");
                return ;
            }
        };
        void init(World *world);
        void setBindingPoint(int port);

        bool fillBufferWithChunks(std::vector<Chunk*> *chunks);
        
};
#endif