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

class MeshBuffer : public DynamicBuffer {
    private:
        CHUNK_MESH_DATATYPE UNACTIVE_MESH_ELEMENT = 393216; // masks face bits to be 6
    protected:
        virtual bool markData(BufferInt markStart, BufferInt markEnd) override;
        
        virtual bool requiresContiguousMemoryLayout() override;
        
        virtual std::string getBufferTypeString() override;
    public:
        GpuBuffer buffer = GpuBuffer(GL_ARRAY_BUFFER);
    protected:
        MeshBuffer(bool deleteData = false, ChunkBufferType bufferType) : DynamicBuffer(&buffer, bufferType, deleteData){
            BUFFER_PADDING = 20; // expressed in %
            CHUNK_PADDING = 10;
            BUFFER_EXPANSION_RATE = 20;
        };
    public:
    
        virtual bool updateChunkBuffer(Chunk* chunk) override;
        virtual bool insertChunksToBuffer(std::vector<Chunk*> *chunks) override;
        
};
#endif