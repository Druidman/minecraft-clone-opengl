#ifndef MESHBUFFER_H
#define MESHBUFFER_H

#include <vector>
#include <utility>
#include <string>

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"

#include "dynamicBuffer.h"
#include "betterGL.h"
#include "chunk.h"
#include "gpuBuffer.h"

#include "opaqueMeshBuffer.h"
#include "transparentMeshBuffer.h"

class MeshBuffer{
    
        
    public:
        GpuBuffer buffer = GpuBuffer(GL_ARRAY_BUFFER);
        OpaqueMeshBuffer opaqueMeshBuffer = OpaqueMeshBuffer(&buffer, true); 
        TransparentMeshBuffer transparentMeshBuffer = TransparentMeshBuffer(&buffer, true);

    public:
        MeshBuffer(){};
        BufferInt getBufferSize(){ return buffer.bufferSize; };

        bool insertChunkToBuffer(Chunk* chunk);
        bool deleteChunkFromBuffer(Chunk* chunk, bool merge = false);
           
        bool updateChunkBuffer(Chunk* chunk);
        bool insertChunksToBuffer(std::vector<Chunk*> *chunks);
        
};
#endif