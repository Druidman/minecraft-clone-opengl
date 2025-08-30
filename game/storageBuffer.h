#ifndef STORAGEBUFFER_H
#define STORAGEBUFFER_H

#include <vector>


#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"

#include "betterGL.h"
#include "dynamicBuffer.h"
#include "chunk.h"
#include "cpuBuffer.h"
#include "gpuBuffer.h"
#include "buffer.h"
#include "world.h"


const int UNIFORM_BUFFER_LENGTH = 1024;
class StorageBuffer : public DynamicBuffer {
    protected:
        
        virtual bool markData(BufferInt markStart, BufferInt markEnd) override;
        virtual BufferInt getChunkDataSize(Chunk* chunk) override{return sizeof(StorageBufferType);};
        virtual bool requiresContiguousMemoryLayout() override {return false;};
        
        virtual std::string getBufferTypeString() override {return "STORAGE_BUFFER";};

    private:
        bool gpuBufferRequiresRefill = false;
        GpuBuffer gpuBuffer = GpuBuffer(GL_UNIFORM_BUFFER);
        CpuBuffer<StorageBufferType> cpuBuffer = CpuBuffer<StorageBufferType>();
        StorageBufferType UNACTIVE_MESH_ELEMENT = glm::vec4(0.0,0.0,0.0,-1.0);
        World *world;

    public:
        StorageBuffer() : DynamicBuffer(&cpuBuffer, STORAGE_BUFFER, false){
            BUFFER_PADDING = 0; // expressed in %
            CHUNK_PADDING = 0;
            BUFFER_EXPANSION_RATE = 0;
        };
        void init(World *world);
        void setBindingPoint(int port);

        virtual bool updateChunkBuffer(Chunk* chunk) override;
        virtual bool insertChunksToBuffer(std::vector<Chunk*> *chunks) override;

        bool fillGpuBuffer();

    
};
#endif