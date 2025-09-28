#include "transparentMeshBuffer.h"


bool TransparentMeshBuffer::markData(BufferInt markStart, BufferInt markEnd)
{
     // now lets mark elements as unactive
    // to mark data as delete we need to change it to have face == 6
    // 393216 - masks face bits to be 6
    
    std::cout << "markStart: " << markStart << " markEnd: " << markEnd << "\n";
    if (
        markEnd < markStart ||
        markStart > this->bufferTarget->bufferSize ||
        markEnd > this->bufferTarget->bufferSize 

    ){
        std::cout  << "FAIL MARK\n";
        return false;
    }
    
    std::vector<CHUNK_MESH_DATATYPE> data((markEnd - markStart) / sizeof(CHUNK_MESH_DATATYPE),UNACTIVE_MESH_ELEMENT);
    
    
    return this->bufferTarget->uploadData(data.data(),data.size() * sizeof(CHUNK_MESH_DATATYPE), markStart); 
}

BufferInt TransparentMeshBuffer::getChunkDataSize(Chunk *chunk)
{
    return chunk->getTransparentMeshSize();
}

bool TransparentMeshBuffer::requiresContiguousMemoryLayout()
{
    return false;
}

std::string TransparentMeshBuffer::getBufferTypeString()
{
    
    return std::string("TRANSPARENT_MESH_BUFFER");
}

bool TransparentMeshBuffer::customUpdateCheck(Chunk* chunk)
{
    std::cout << "TRANS CHECK\n";
    if (chunk->getTransparentMesh()->size() != 0){
        return true;
    }
    return false;
}

bool TransparentMeshBuffer::updateChunkBuffer(Chunk *chunk)
{
    if (!customUpdateCheck(chunk)){
        return true;
    };
    if (!chunk->hasBufferSpace[this->chunkBufferType]){
       
        return true;
    }
    BufferInt meshSize = getChunkDataSize(chunk);
    if (meshSize + chunk->bufferZone[this->chunkBufferType].first > chunk->bufferZone[this->chunkBufferType].second ){
        // chunk is to big so we either find new free zone OR reallocate buffer

        // ! in all of these scenarios we need to remove chunk from its current location !
        if (!deleteChunkFromBuffer(chunk)){
            ExitError(getBufferTypeString(),"Error deleting chunk in update func");
        };

        // lets try to find new freeZone

        int assignRes = assignChunkBufferZone(chunk);
        if (assignRes == -2){
            // we need to expand buffer because we have no usable space left
            
            if (!expandBufferByChunk(chunk)){
                ExitError(getBufferTypeString(),"Buffer expansion in update func");
            };
        }
        else if (assignRes == -1){
            std::cout << "update -1\n";
            return false;
        }

        
        return updateChunkBuffer(chunk);

    }


    if (chunk->bufferZone[this->chunkBufferType].second < chunk->bufferZone[this->chunkBufferType].first){
        ExitError(getBufferTypeString(),"smth wrong with chunk free zones second < first, UPDATECHUNKBUFFER(), UPDATING");
        return false;
    }
    markData(chunk->bufferZone[this->chunkBufferType].first, chunk->bufferZone[this->chunkBufferType].second);
    if (chunk->getTransparentMesh()->size() != 0){
        if (!this->bufferTarget->uploadData(
            chunk->getTransparentMesh()->data(), 
            chunk->getTransparentMesh()->size() * sizeof(CHUNK_MESH_DATATYPE), 
            chunk->bufferZone[this->chunkBufferType].first
        )){
            return false;
        }
        this->bufferCalls++;
    }
    
    
    
    return true;
}

bool TransparentMeshBuffer::insertChunksToBuffer(std::vector<Chunk *> *chunks)
{
    return false;
}
