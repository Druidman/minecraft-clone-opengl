#ifndef TRANSPARENT_MESH_BUFFER_H
#define TRANSPARENT_MESH_BUFFER_H
#include "meshBuffer.h"

class TransparentMeshBuffer : public MeshBuffer {
    protected:
        virtual BufferInt getChunkDataSize(Chunk* chunk) override;

    public:
        TransparentMeshBuffer() : MeshBuffer(true, MESH_TRANSPARENT_BUFFER){

        }
    
};
#endif