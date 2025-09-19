#include "meshBuffer.h"

bool MeshBuffer::allocateBuffer(BufferInt opaqueSize, BufferInt transparentSize){
    
    BufferInt realOpaqueSize = (opaqueSize * 1.2);
    BufferInt realTransparentSize = (transparentSize * 1.2);

    realOpaqueSize -= realOpaqueSize % 2;
    realTransparentSize -= realTransparentSize % 2;
    
    if (!this->buffer.allocateBuffer(realOpaqueSize + realTransparentSize)){
        ExitError("MESH_BUFFER", "SMTH WRONG WITH alloc");
    };

    this->opaqueMeshSubBuffer = SubBuffer(
        0,
        realOpaqueSize, 
        &this->buffer
    );
    this->transparentMeshSubBuffer = SubBuffer(
        realOpaqueSize, 
        this->buffer.bufferSize, 
        &this->buffer
    );

    this->opaqueMeshBuffer = OpaqueMeshBuffer(&opaqueMeshSubBuffer, true); 
    this->transparentMeshBuffer = TransparentMeshBuffer(&transparentMeshSubBuffer, true);

    
    this->opaqueMeshBuffer.clearBuffer();
    this->transparentMeshBuffer.clearBuffer();
    return true;
}

bool MeshBuffer::insertChunkToBuffer(Chunk* chunk){
    bool res = true;
    res &= this->opaqueMeshBuffer.insertChunkToBuffer(chunk);
    res &= this->transparentMeshBuffer.insertChunkToBuffer(chunk);
    return res;
}
bool MeshBuffer::deleteChunkFromBuffer(Chunk* chunk, bool merge){
    bool res = true;
    res &= this->opaqueMeshBuffer.deleteChunkFromBuffer(chunk, merge);
    res &= this->transparentMeshBuffer.deleteChunkFromBuffer(chunk, merge);
    return res;
}
bool MeshBuffer::updateChunkBuffer(Chunk* chunk){
    bool res = true;
    res &= this->opaqueMeshBuffer.updateChunkBuffer(chunk);
    res &= this->transparentMeshBuffer.updateChunkBuffer(chunk);
    return res;
};
bool MeshBuffer::insertChunksToBuffer(std::vector<Chunk*> *chunks){
    bool res = true;
    res &= this->opaqueMeshBuffer.insertChunksToBuffer(chunks);
    res &= this->transparentMeshBuffer.insertChunksToBuffer(chunks);

    return res;
};