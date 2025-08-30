#ifndef ID_BUFFER_H
#define ID_BUFFER_H
#include <vector>

#include "dynamicBuffer.h"
#include "gpuBuffer.h"
#include "chunk.h"
#include "buffer.h"


class IdBuffer : public DynamicBuffer{
    protected:
        
        virtual bool markData(BufferInt markStart, BufferInt markEnd) override;
        virtual BufferInt getChunkDataSize(Chunk* chunk) override{return (chunk->getMeshSize() / sizeof(CHUNK_MESH_DATATYPE)) * sizeof(int);};
        virtual bool requiresContiguousMemoryLayout() override {return false;};
        
        virtual std::string getBufferTypeString() override {return "ID_BUFFER";};

    private:
        bool gpuBufferRequiresRefill = false;
        int UNACTIVE_MESH_ELEMENT = -1;
    public:
        GpuBuffer gpuBuffer = GpuBuffer(GL_ARRAY_BUFFER);
        CpuBuffer<int> cpuBuffer = CpuBuffer<int>();
        


    public:
        IdBuffer() : DynamicBuffer(&cpuBuffer, ID_BUFFER, true){
            BUFFER_PADDING = 20; // expressed in %
            CHUNK_PADDING = 10;
            BUFFER_EXPANSION_RATE = 20;
        };
        

        virtual bool updateChunkBuffer(Chunk* chunk) override;
        virtual bool insertChunksToBuffer(std::vector<Chunk*> *chunks) override;

        bool fillGpuBuffer();
};

#endif