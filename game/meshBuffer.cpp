#include "meshBuffer.h"

BufferInt MeshBuffer::getChunkDataSize(Chunk *chunk)
{
    return chunk->getMeshSize();
}

bool MeshBuffer::updateChunkBuffer(Chunk *chunk)
{
    BufferInt meshSize = getChunkDataSize(chunk);
    if (meshSize > chunk->bufferZone[bufferType].second - chunk->bufferZone[bufferType].first){
        // chunk is to big so we either find new free zone OR reallocate buffer
        ExitError("BUffer","can't delete");
        return false;

        // ! in all of these scenarios we need to remove chunk from its current location !
        deleteChunkFromBuffer(chunk);

        // lets try to find new freeZone

        int assignRes = assignChunkBufferZone(chunk);
        if (assignRes == -2){
            // we need to expand buffer because we have no usable space left
            expandBufferByChunk(chunk);
        }
        else if (assignRes == -1){
            std::cout << "update -1\n";
            return false;
        }

        insertChunkToBuffer(chunk);
    }
    std::cout << "Mesh Buffer insertion: \n";
    std::cout << "Buffer Zone\n";
    std::cout << "First: " << chunk->bufferZone[bufferType].first;
    std::cout << "\nSecond: " << chunk->bufferZone[bufferType].second;
    std::cout << "\n";
    BufferInt transMeshOffset = chunk->getOpaqueMesh()->size() * sizeof(CHUNK_MESH_DATATYPE);
    if (chunk->getOpaqueMesh()->size() != 0){
        if (!updateData<CHUNK_MESH_DATATYPE>(
            chunk->getOpaqueMesh(), 
            chunk->bufferZone[bufferType].first, 
            chunk->bufferZone[bufferType].second
        )){
            return false;
        }
    }
    if (chunk->getTransparentMesh()->size() != 0){
        if (!updateData<CHUNK_MESH_DATATYPE>(
            chunk->getTransparentMesh(), 
            chunk->bufferZone[bufferType].first + transMeshOffset, 
            chunk->bufferZone[bufferType].second
        )){
            return false;
        }
    }
    
    
    return true;
}