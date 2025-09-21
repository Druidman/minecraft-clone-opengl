#ifndef OPAQUE_MESH_BUFFER_H
#define OPAQUE_MESH_BUFFER_H

#include "dynamicBuffer.h"

class OpaqueMeshBuffer : public DynamicBuffer{
    private:
        CHUNK_MESH_DATATYPE UNACTIVE_MESH_ELEMENT = 393216; // masks face bits to be 6
    protected:
        virtual bool markData(BufferInt markStart, BufferInt markEnd) override;
        virtual BufferInt getChunkDataSize(Chunk* chunk) override;
        virtual bool requiresContiguousMemoryLayout() override;
        

        virtual std::string getBufferTypeString() override;
    
    public:
        OpaqueMeshBuffer(Buffer* buffer, bool deleteData = false) : DynamicBuffer(buffer, OPAQUE_MESH_BUFFER, deleteData){
            BUFFER_PADDING = 20; // expressed in %
            CHUNK_PADDING = 10;
            BUFFER_EXPANSION_RATE = 20;
            
        };
    
        virtual bool updateChunkBuffer(Chunk* chunk) override;
        virtual bool insertChunksToBuffer(std::vector<Chunk*> *chunks) override;
};

#endif