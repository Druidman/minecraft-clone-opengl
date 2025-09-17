#include "meshBuffer.h"

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