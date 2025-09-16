#ifndef OPAQUE_MESH_BUFFER_H
#define OPAQUE_MESH_BUFFER_H
#include "meshBuffer.h"

class OpaqueMeshBuffer : public MeshBuffer {
    protected:
        virtual BufferInt getChunkDataSize(Chunk* chunk) override;

    public:
        OpaqueMeshBuffer() : MeshBuffer(true, MESH_OPAQUE_BUFFER){

        }
    
};
#endif