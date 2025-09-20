#ifndef TRANSPARENT_ID_BUFFER_H
#define TRANSPARENT_ID_BUFFER_H

#include "dynamicIdBuffer.h"

class TransparentIdBuffer : public DynamicIdBuffer {
    protected:
        virtual BufferInt getChunkDataSize(Chunk *chunk) override
        {
            return (chunk->getTransparentMeshSize() / sizeof(CHUNK_MESH_DATATYPE)) * sizeof(int);
        }
    public:
        TransparentIdBuffer(Buffer* buffer, bool deleteData = false) : DynamicIdBuffer(buffer, TRANSPARENT_ID_BUFFER, deleteData){};
};

#endif