#include "meshBuffer.h"
#include "dynamicBuffer.h"

bool MeshBuffer::markData(BufferInt markStart, BufferInt markEnd)
{
     // now lets mark elements as unactive
    // to mark data as delete we need to change it to have face == 6
    // 393216 - masks face bits to be 6
    if (
        markEnd < markStart ||
        markStart > this->bufferTarget->bufferSize ||
        markEnd > this->bufferTarget->bufferSize
    ){
        std::cout  << "FAIL MARK\n";
        return false;
    }
    
    std::vector<CHUNK_MESH_DATATYPE> data((markEnd - markStart) / sizeof(CHUNK_MESH_DATATYPE),UNACTIVE_MESH_ELEMENT);
 
    return this->bufferTarget->uploadData(data.data(),markEnd - markStart, markStart); 
}

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
    if (!chunk->hasBufferSpace[this->chunkBufferType]){
        ExitError("MESH_BUFFER","Calling update on not inserted chunk");
        return false;
    }
    BufferInt meshSize = getChunkDataSize(chunk);
    if (meshSize + chunk->bufferZone[this->chunkBufferType].first > chunk->bufferZone[this->chunkBufferType].second ){
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

        
        return updateChunkBuffer(chunk);

    }
    BufferInt transMeshOffset = chunk->getOpaqueMesh()->size() * sizeof(CHUNK_MESH_DATATYPE);


    if (chunk->bufferZone[this->chunkBufferType].second < chunk->bufferZone[this->chunkBufferType].first){
        ExitError("DYNAMIC_BUFFER","smth wrong with chunk free zones second < first, UPDATECHUNKBUFFER(), UPDATING");
        return false;
    }
    markData(chunk->bufferZone[this->chunkBufferType].first, chunk->bufferZone[this->chunkBufferType].second);
    if (chunk->getOpaqueMesh()->size() != 0){
        if (!this->bufferTarget->uploadData(
            chunk->getOpaqueMesh()->data(), 
            chunk->getOpaqueMesh()->size() * sizeof(CHUNK_MESH_DATATYPE), 
            chunk->bufferZone[this->chunkBufferType].first
        )){
            return false;
        }
        this->bufferCalls++;
    }
    if (chunk->getTransparentMesh()->size() != 0){
        if (!this->bufferTarget->uploadData(
            chunk->getTransparentMesh()->data(), 
            chunk->getTransparentMesh()->size() * sizeof(CHUNK_MESH_DATATYPE), 
            chunk->bufferZone[this->chunkBufferType].first + transMeshOffset
        )){
            return false;
        }
        this->bufferCalls++;
    }
    
    
    return true;
}

bool MeshBuffer::insertChunksToBuffer(std::vector<Chunk *> *chunks)
{
    return false;
}
