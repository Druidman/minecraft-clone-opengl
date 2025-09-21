#ifndef OPAQUE_ID_BUFFER_H
#define OPAQUE_ID_BUFFER_H

#include "dynamicIdBuffer.h"

class OpaqueIdBuffer : public DynamicIdBuffer {
    protected:
        virtual BufferInt getChunkDataSize(Chunk *chunk) override
        {
            return (chunk->getOpaqueMeshSize() / sizeof(CHUNK_MESH_DATATYPE)) * sizeof(int);
        }
    public:
        OpaqueIdBuffer(Buffer* buffer, bool deleteData = false) : DynamicIdBuffer(buffer, OPAQUE_ID_BUFFER, deleteData){};
};

#endif
