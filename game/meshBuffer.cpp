#include "meshBuffer.h"

BufferInt MeshBuffer::getChunkDataSize(Chunk *chunk)
{
    return chunk->getMeshSize();
}

bool MeshBuffer::requiresContiguousMemoryLayout()
{
    return false;
}

std::string MeshBuffer::getBufferTypeString()
{
    return std::string("MESH_BUFFER");
}

bool MeshBuffer::updateChunkBuffer(Chunk *chunk)
{
    if (!chunk->hasBufferSpace[bufferType]){
        ExitError("MESH_BUFFER","Calling update on not inserted chunk");
        return false;
    }
    BufferInt meshSize = getChunkDataSize(chunk);
    if (meshSize + chunk->bufferZone[bufferType].first > chunk->bufferZone[bufferType].second ){
        // chunk is to big so we either find new free zone OR reallocate buffer

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

        
        return insertChunkToBuffer(chunk);

    }
    BufferInt transMeshOffset = chunk->getOpaqueMesh()->size() * sizeof(CHUNK_MESH_DATATYPE);


    if (chunk->bufferZone[bufferType].second < chunk->bufferZone[bufferType].first){
        ExitError("DYNAMIC_BUFFER","smth wrong with chunk free zones second < first, UPDATECHUNKBUFFER(), UPDATING");
        return false;
    }
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

bool MeshBuffer::insertChunksToBuffer(std::vector<Chunk *> *chunks)
{
    return false;
}
