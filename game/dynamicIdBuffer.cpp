#include "dynamicIdBuffer.h"

bool DynamicIdBuffer::markData(BufferInt markStart, BufferInt markEnd)
{
    if (
        markEnd < markStart ||
        markStart > this->bufferTarget->bufferSize ||
        markEnd > this->bufferTarget->bufferSize 

    ){
        std::cout  << "FAIL MARK\n";
        return false;
    }
    
    std::vector<int> data((markEnd - markStart) / sizeof(int),UNACTIVE_ELEMENT);
 
    return this->bufferTarget->uploadData(data.data(),data.size() * sizeof(int), markStart); 
}



bool DynamicIdBuffer::requiresContiguousMemoryLayout()
{
    return false;
}

std::string DynamicIdBuffer::getBufferTypeString()
{
    return "DYNAMIC_ID_BUFFER";
}

bool DynamicIdBuffer::updateChunkBuffer(Chunk *chunk)
{
    if (!chunk->hasBufferSpace[this->chunkBufferType]){
        ExitError(getBufferTypeString(),"UPDATE CALLED ON NOT INSERTED CHUNK");
    }
    BufferInt dataSize = getChunkDataSize(chunk);
    if (chunk->bufferZone[this->chunkBufferType].first + dataSize > chunk->bufferZone[this->chunkBufferType].second){
        // chunk is to big so we either find new free zone OR reallocate buffer

        // ! in all of these scenarios we need to remove chunk from its current location !
        if (!deleteChunkFromBuffer(chunk)){
            ExitError(getBufferTypeString(),"Error deleting chunk in update func");
        };

        // lets try to find new freeZone
        std::cout << "assigning new zone for a chunk in DYNAMICIDBUFFER\n";
        int assignRes = assignChunkBufferZone(chunk);
        if (assignRes == -2){
            // we need to expand buffer because we have no usable space left
            
            if (!expandBufferByChunk(chunk)){
                ExitError(getBufferTypeString(),"Error expanding buffer in update func");
            };
        }
        else if (assignRes == -1){
            std::cout << "update -1\n";
            return false;
        }

        
        return updateChunkBuffer(chunk);
    }
    
    std::vector<int> data(
        (chunk->bufferZone[this->chunkBufferType].second - chunk->bufferZone[this->chunkBufferType].first) / sizeof(int),
        chunk->bufferZone[STORAGE_BUFFER].first / sizeof(StorageBufferType)
    );
    

    if (!this->bufferTarget->uploadData(
            data.data(),
            chunk->bufferZone[this->chunkBufferType].second - chunk->bufferZone[this->chunkBufferType].first,
            chunk->bufferZone[this->chunkBufferType].first
        )
    ){
        return false;
    };
    postChunkUpdateFunction();
    return true;
}

bool DynamicIdBuffer::insertChunksToBuffer(std::vector<Chunk *> *chunks)
{
    if (chunks->size() == 0){
        return false;
    }

    for (Chunk* chunk : *chunks){
        if (!insertChunkToBuffer(chunk)){
            return false;
        }
    }
    return true;
}
